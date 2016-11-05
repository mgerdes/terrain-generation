#!/bin/python
import triangle
import numpy
import noise as pnoise
import random
from PIL import Image 

def make_noise_for_biomes():
    num_biomes = 4 
    n = make_noise(1)

    for i in range(600):
        for j in range(600):
            if (n[i][j] < 0.1):
                n[i][j] = 0.0
            elif (n[i][j] < 0.5):
                n[i][j] = 0.1
            elif (n[i][j] < 0.6):
                n[i][j] = 0.3
            else:
                n[i][j] = 1.0
    return n

def make_noise(f = 20):
    n_min = 100
    n_max = -100
    n = []

    x_off = 1000 * random.random()
    y_off = 1000 * random.random()

    for i in range(600):
        n.append([]);
        for j in range(600):
            n_val = pnoise.pnoise2(x_off + f * i / 600, y_off + f * j / 600, 3)
            if (n_val < n_min): 
                n_min = n_val
            if (n_val > n_max): 
                n_max = n_val
            n[i].append(n_val)

    for i in range(600):
        for j in range(600):
            n[i][j] = (n[i][j] - n_min) / (n_max - n_min)

    return n

def output_noise_image(n, filename):
    img = Image.new('RGB', (600, 600))

    for i in range(600):
        for j in range(600):
            a = int(n[i][j] * 256);
            img.putpixel((i, j), (a, a, a))

    img.save(filename)

def normalize_number(n, low, high):
    return low + n * (high - low);

def noise_combine_with_biome(noise, biome_noise):
    new_noise = []

    for i in range(600):
        new_noise.append([])
        for j in range(600):
            new_noise[i].append(0)

    for i in range(600):
        for j in range(600):
            if (biome_noise[i][j] == 0.0):
                new_noise[i][j] = normalize_number(noise[i][j], 0.00, 0.05)
            elif (biome_noise[i][j] == 0.1):
                new_noise[i][j] = normalize_number(noise[i][j], 0.05, 0.10)
            elif (biome_noise[i][j] == 0.3):
                new_noise[i][j] = normalize_number(noise[i][j], 0.10, 0.30)
            else:
                new_noise[i][j] = normalize_number(noise[i][j], 0.30, 1.00)

    return new_noise


def noise_get_point(n, i, j):
    if (i < 0 or j < 0 or i >= 600 or j >= 600):
        return 0

    return n[i][j]

def noise_sample_points(n):
    pts = []

    for i in range(600):
        for j in range(600):
            p0 = noise_get_point(n, i, j)
            p1 = noise_get_point(n, i - 1, j)
            p2 = noise_get_point(n, i + 1, j)
            p3 = noise_get_point(n, i, j - 1)
            p4 = noise_get_point(n, i, j + 1)

            d = 0.2 * (abs(p1 - p0) + abs(p2 - p0) + abs(p3 - p0) + abs(p4 - p0))

            if (random.random() < 0.05):
                pts.append((i, j)) 
            else:
                if (d > 0.005 and random.random() > 0.4):
                    pts.append((i, j)) 

    return pts

def output_sampled_points_image(pts, filename):
    img = Image.new('RGB', (600, 600))

    for i in range(600):
        for j in range(600):
            img.putpixel((i, j), (255, 255, 255))

    for pt in pts:
        img.putpixel(pt, (0, 0, 0))

    img.save(filename)

def get_color_for_triangle(tri):
    elev = 0.3 * (tri[0][1] + tri[1][1] + tri[2][1]) / 100

    normal = numpy.cross(numpy.subtract(tri[0], tri[2]), numpy.subtract(tri[0], tri[1]))
    normal = normal / numpy.linalg.norm(normal)
    slope = max(numpy.dot(normal, [0, 1, 0]), 0.0)

    if (slope < 0.5):
        return (0.3, 0.3, 0.3)

    if (elev < 0.05):
        return (0, 0.41, 0.58)
    elif (elev < 0.10):
        return (0, 1, 0)
    elif (elev < 0.30):
        return (0.2, 0.2, 0.2)
    else:
        return (1.0, 1.0, 1.0)

def create_mesh_from_points(pts, noise):
    f = open('terrain_mesh.bin', 'w')
    tris = triangle.delaunay(pts)

    for i in range(600):
        for j in range(600):
            elev = 100 * noise[i][j]
            f.write(str(elev) + '\n')

    f.write(str(len(tris)) + '\n')
    for tri in tris:
        t = []
        for i in range(3):
            pt = pts[tri[i]]
            elev = 100 * noise[pt[0]][pt[1]]
            t.append([pt[0], elev, pt[1]])
        color = get_color_for_triangle(t)
        for pt in t:
            f.write(str(pt[0]) + ' ' + str(pt[1]) + ' ' + str(pt[2]) + '\n')
            f.write(str(color[0]) + ' ' + str(color[1]) + ' ' + str(color[2]) + '\n')

noise = make_noise()
#output_noise_image(noise, 'noise.png')
biome_noise = make_noise_for_biomes()
#output_noise_image(biome_noise, 'biome_noise.png')
terrain_noise = noise_combine_with_biome(noise, biome_noise)
#output_noise_image(terrain_noise, 'terrain_noise.png')
pts = noise_sample_points(terrain_noise)
#output_sampled_points_image(pts, 'terrain_pts.png')
create_mesh_from_points(pts, terrain_noise)
