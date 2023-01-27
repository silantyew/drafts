import tkinter as tk
from tkinter import ttk
import numpy as np
from geometry import *



CANVAS_WIDTH_PX, CANVAS_HEIGHT_PX = ( 800, 600 )
POINT_RADIUS = 2

model = None
projection_type = None


def draw_point(point):
    x1, y1 = (point[X] - POINT_RADIUS), (point[Y] - POINT_RADIUS)
    x2, y2 = (point[X] + POINT_RADIUS), (point[Y] + POINT_RADIUS)
    canvas.create_oval(x1, y1, x2, y2, fill = 'red')

def draw_line(line, color):
    canvas.create_line(line[0][X], line[0][Y], line[1][X], line[1][Y], fill = color)

def draw_model(model, display_vertexes = True, color = 'black'):
    for l in model:
        draw_line(l, color)
        
        if display_vertexes:
            for v in l:
                draw_point(v)

def redraw():
    global model
    
    canvas.delete('all')
    
    alpha, beta, gamma = slider_x_axis_rotation.get(), \
                         slider_y_axis_rotation.get(), \
                         slider_z_axis_rotation.get()

    axis_len = 20
    origin = Point3D(0, 0, 0)
    origin_model = Model([ Line(origin, Point3D(axis_len, 0, 0)), \
                           Line(origin, Point3D(0, axis_len, 0)), \
                           Line(origin, Point3D(0, 0, axis_len)) ])
    rotate(origin_model, alpha, beta, gamma)
    draw_model(origin_model, display_vertexes = False, color = 'green')


    h, R = 300, 100
    model = Prism(slider_vertexes.get(), R, h)
    translate(model, 0, 0, -h / 2)
    rotate(model, alpha, beta, gamma)
    draw_model(model, display_vertexes = checkbtn_display_vertexes.instate(['selected']))

def draw_projection():
    canvas.delete('all')
    draw_model(projection(model, projection_type.get()))
    


root = tk.Tk ()
projection_type = tk.StringVar()


canvas = tk.Canvas(width = CANVAS_WIDTH_PX, height = CANVAS_HEIGHT_PX)
canvas.grid(row = 0, column = 0, columnspan = 10)
canvas.configure(scrollregion = (-CANVAS_WIDTH_PX // 2, -CANVAS_HEIGHT_PX // 2, \
                                 CANVAS_WIDTH_PX // 2, CANVAS_HEIGHT_PX // 2))
canvas.xview_moveto(.5)
canvas.yview_moveto(.5)


tk.Label(root, text = 'Vertexes:').grid(row = 2, column = 0)


slider_vertexes = tk.Scale(root, from_ = 3, to = 20, orient = 'horizontal', \
                            command = lambda event: redraw())
slider_vertexes.set(20)
slider_vertexes.grid(row = 1, column = 1, rowspan = 2)


tk.Label(root, text = 'X-axis rotation (deg):').grid(row = 3, column = 0)


slider_x_axis_rotation = tk.Scale(root, from_ = 0, to = 360, orient = 'horizontal', \
                            command = lambda event: redraw())
slider_x_axis_rotation.grid(row = 3, column = 1, rowspan = 2)


tk.Label(root, text = 'Y-axis rotation (deg):').grid(row = 5, column = 0)


slider_y_axis_rotation = tk.Scale(root, from_ = 0, to = 360, orient = 'horizontal', \
                            command = lambda event: redraw())
slider_y_axis_rotation.grid(row = 5, column = 1, rowspan = 2)


tk.Label(root, text = 'Z-axis rotation, deg:').grid(row = 7, column = 0)


slider_z_axis_rotation = tk.Scale(root, from_ = 0, to = 360, orient = 'horizontal', \
                            command = lambda event: redraw())
slider_z_axis_rotation.grid(row = 7, column = 1, rowspan = 2)


checkbtn_display_vertexes = ttk.Checkbutton(root, text = 'Display vertexes', \
                            command = redraw)
checkbtn_display_vertexes.state(['selected'])
checkbtn_display_vertexes.grid(row = 9, column = 0)


lblframe_projections = tk.LabelFrame(root, text = 'Projections')
lblframe_projections.grid(row = 1, column = 8, rowspan = 5)


radiobtn_horizontal_projection = tk.Radiobutton(lblframe_projections, text = 'Horizontal', \
                                                var = projection_type, value = 'horizontal')
radiobtn_horizontal_projection.grid(row = 5, column = 8)


radiobtn_frontal_projection = tk.Radiobutton(lblframe_projections, text = 'Frontal', \
                                                var = projection_type, value = 'frontal')
radiobtn_frontal_projection.grid(row = 6, column = 8)


radiobtn_profile_projection = tk.Radiobutton(lblframe_projections, text = 'Profile', \
                                                var = projection_type, value = 'profile')
radiobtn_profile_projection.grid(row = 7, column = 8)


radiobtn_draw_projection = tk.Button(lblframe_projections, text = 'Draw', command = draw_projection)
radiobtn_draw_projection.grid(row = 8, column = 8)


projection_type.set('horizontal') # default value
#redraw() # will be called by events after tk.mainloop()
tk.mainloop()
