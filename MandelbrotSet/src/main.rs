extern crate num;
extern crate image;

use num::Complex;
use image::ColorType;
use image::png::PNGEncoder;
use std::fs::File;

fn escape_time(c: Complex<f64>, max_iterations: u32) -> Option<u32>
{
    let mut z = Complex { re: 0.0, im: 0.0 };
    for i in 0..max_iterations {
        z = z*z + c;
        if z.norm_sqr() > 4.0 {
            return Some(i);
        }
    }
    
    None
}

fn pixel_to_point(bounds: (usize, usize),
                  pixel: (usize, usize),
                  upper_left: Complex<f64>,
                  lower_right: Complex<f64>)
   -> Complex<f64>
{
    let (width, height) = (lower_right.re - upper_left.re,
                           upper_left.im - lower_right.im);
                           
    Complex {
        re: upper_left.re + pixel.0 as f64 * (width / bounds.0 as f64),
        im: upper_left.im - pixel.1 as f64 * (height / bounds.1 as f64)
    }
}

fn render(pixels: &mut [u8],
          bounds: (usize, usize),
          upper_left: Complex<f64>,
          lower_right: Complex<f64>)
{
    assert!(pixels.len() == bounds.0 * bounds.1);
    
    for row in 0..bounds.1 {
        for column in 0..bounds.0 {
            let point = pixel_to_point(bounds, (column, row),
                                       upper_left, lower_right);
            
            pixels[row * bounds.0 + column] = 
                match escape_time(point, 255) {
                    None => 0,
                    Some(count) => 255 - count as u8
                };
        }
    }
}

fn write_image(filename: &str, pixels: &[u8], bounds: (usize, usize))
   -> Result<(), std::io::Error>
{
    let output = File::create(filename)?;
    let encoder = PNGEncoder::new(output);
    encoder.encode(&pixels,
                   bounds.0 as u32, bounds.1 as u32,
                   ColorType::Gray(8))?;
    Ok(())
}

fn main() {
    let bounds = (1000, 1000);
    let upper_left = Complex { re: -2.0, im: 1.5 };
    let lower_right = Complex { re: 1.0, im: -1.5 };
    let filename = "mandelbrot.png";
    
    let mut pixels = vec![0; bounds.0 * bounds.1];
    
    render(&mut pixels, bounds, upper_left, lower_right);
    write_image(filename, &pixels, bounds).expect("Error writing PNG file");
}
