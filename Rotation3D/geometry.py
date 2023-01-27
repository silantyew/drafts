import numpy as np
import math
from math import sin, cos

X, Y, Z = 0, 1, 2
projections = { 'horizontal': (X, Y), 'frontal': (X, Z), 'profile': (Y, Z) }


def Point2D(x = 0, y = 0,):
    return np.array([x, y])
    
def Point3D(x = 0, y = 0, z = 0):
    return np.array([x, y, z])

def Line(p1, p2):
    return np.array([p1, p2])
    
def Model(lines):
    return np.array(lines)

def Prism(num_of_vertexes, R, h):
    bottom_bases_vertexes = []
    top_bases_vertexes = []
    
    angle = 0
    for i in range(num_of_vertexes):
        v = Point3D(R * cos(math.radians(angle)), \
                    R * sin(math.radians(angle)), \
                    0)
                  
        bottom_bases_vertexes.append(v)
        top_bases_vertexes.append(np.add(v, [0, 0, h]))
        
        angle += 360 / num_of_vertexes
        
        
    lines = []
    
    for i in range(num_of_vertexes):
        lines.append(Line(bottom_bases_vertexes[i - 1], bottom_bases_vertexes[i]))
        lines.append(Line(top_bases_vertexes[i - 1], top_bases_vertexes[i]))
        lines.append(Line(bottom_bases_vertexes[i], top_bases_vertexes[i]))
    
    return Model(lines)


def rotate(model, alpha, beta, gamma):
    alpha, beta, gamma = math.radians(alpha), math.radians(beta), math.radians(gamma)
    
    rotation_matrix = np.array([ [ cos(beta) * cos(gamma), -sin(gamma) * cos(beta), sin(beta) ], \
                                 [ sin(alpha) * sin(beta) * cos(gamma) + sin(gamma) * cos(alpha), \
                                   -sin(alpha) * sin(beta) * sin(gamma) + cos(alpha) * cos(gamma), \
                                   -sin(alpha) * cos(beta) ], \
                                 [ sin(alpha) * sin(gamma) - sin(beta) * cos(alpha) * cos(gamma), \
                                   sin(alpha) * cos(gamma) + sin(beta) * sin(gamma) * cos(alpha), \
                                   cos(alpha) * cos(beta) ] ])

    for v in range(len(model)):
        model[v] = np.dot(model[v], rotation_matrix)


def translate(model, dx, dy, dz):
    translation_matrix = [ dx, dy, dz ]
    
    for v in range(len(model)):
         model[v] = np.add(model[v], translation_matrix)
         
         
def projection(model, type_):
    coords = projections[type_]
    
    lines = []
    for l in model:
        lines.append(Line(Point2D(l[0][coords[X]], l[0][coords[Y]]), \
                          Point2D(l[1][coords[X]], l[1][coords[Y]])))
    
    return np.array(lines)
