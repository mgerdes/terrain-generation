#!/bin/python
import triangle
import numpy
import noise as pnoise
import random
import math
from PIL import Image, ImageDraw

width = 2000

biomes = [ 
        {'transition_range': (0.00, 0.00), 'in_range': (0.00, 0.20), 'out_range': (0.00, 0.05)}, # WATER
        {'transition_range': (0.20, 0.20), 'in_range': (0.20, 0.48), 'out_range': (0.05, 0.10)}, # GRASS
        {'transition_range': (0.48, 0.50), 'in_range': (0.50, 0.60), 'out_range': (0.10, 0.30)}, # ROCKS
        {'transition_range': (0.60, 0.65), 'in_range': (0.65, 1.00), 'out_range': (0.30, 1.00)}  # MOUNTAIN
        ]

def make_noise(freq, save_image = False, file_name = 'noise.png'):
    noise = []
    for i in range(width):
        noise.append([])
        for j in range(width):
            noise[i].append(0)

    i_off = 1000 * random.random()
    j_off = 1000 * random.random()

    noise_min = 100
    noise_max = -100

    for i in range(width):
        for j in range(width):
            val = pnoise.pnoise2(i_off + freq * i / width, j_off + freq * j / width, 3)
            if (val < noise_min): 
                noise_min = val
            if (val > noise_max): 
                noise_max = val
            noise[i][j] = val

    for i in range(width):
        for j in range(width):
            noise[i][j] = (noise[i][j] - noise_min) / (noise_max - noise_min)

    if (save_image):
        img = Image.new('RGB', (width, width))
        for i in range(width):
            for j in range(width):
                val = int(noise[i][j] * 256);
                img.putpixel((i, j), (val, val, val))
        img.save(file_name)

    return noise

def make_biome_map(save_image = False, file_name = 'biome_map.png'):
    noise = make_noise(width / 500)
    bezier_path = create_bezier_path(True)

    biome_map = []
    for i in range(width):
        biome_map.append([])
        for j in range(width):
            biome_map[i].append(0)

    for i in range(width):
        for j in range(width):
            if (bezier_path[i][j] != 0):
                biome_map[i][j] = (255 - bezier_path[i][j]) / 256
            else:
                for k in range(len(biomes)):
                    biome = biomes[k]

                    trans_low = biome['transition_range'][0]
                    trans_high = biome['transition_range'][1]

                    in_low = biome['in_range'][0]
                    in_high = biome['in_range'][1]

                    if (k > 0 and noise[i][j] >= trans_low and noise[i][j] < trans_high):
                        biome_map[i][j] = (k - 1) + (noise[i][j] - trans_low) / (trans_high - trans_low)

                    if (noise[i][j] >= in_low and noise[i][j] < in_high):
                        biome_map[i][j] = k

    if (save_image):
        img = Image.new('RGB', (width, width))
        for i in range(width):
            for j in range(width):
                val = int((biome_map[i][j] / len(biomes)) * 256);
                img.putpixel((i, j), (val, val, val))
        img.save(file_name)

    return biome_map

def normalize_number(n, low, high):
    return low + n * (high - low);

def create_terrain_height_map(save_image = False, file_name = 'height_map.png'):
    height_map = []
    for i in range(width):
        height_map.append([])
        for j in range(width):
            height_map[i].append(0)

    biome_map = make_biome_map(True)
    noise = make_noise(width / 500 * 12)

    for i in range(width):
        for j in range(width):
            if (int(biome_map[i][j]) == biome_map[i][j]):
                biome = biomes[int(biome_map[i][j])]
                
                low = biome['out_range'][0]
                high = biome['out_range'][1]

                height_map[i][j] = normalize_number(noise[i][j], low, high)
            else:
                biome0 = biomes[int(biome_map[i][j])]
                biome1 = biomes[int(biome_map[i][j]) + 1]

                low0 = biome0['out_range'][0]
                high0 = biome0['out_range'][1]
                height0 = normalize_number(noise[i][j], low0, high0)

                low1 = biome1['out_range'][0]
                high1 = biome1['out_range'][1]
                height1 = normalize_number(noise[i][j], low1, high1)
                
                t0 = int(biome_map[i][j] + 1) - biome_map[i][j]
                t1 = biome_map[i][j] - int(biome_map[i][j])

                height_map[i][j] = t0 * height0 + t1 * height1

    if (save_image):
        img = Image.new('RGB', (width, width))
        for i in range(width):
            for j in range(width):
                val = int(height_map[i][j] * 256)
                img.putpixel((i, j), (val, val, val))
        img.save(file_name)

    return height_map


def create_terrain_mesh(save_image = True, file_name = 'terrain_mesh.png'):
    height_map = create_terrain_height_map(save_image);
    voronoi_diagram = create_voronoi_diagram(25)

    def get_point_on_height_map(i, j):
        if (i < 0 or j < 0 or i >= width or j >= width):
            return 0
        return height_map[i][j]

    def get_triangle_color(tri):
        elev = 0.3 * (tri[0][1] + tri[1][1] + tri[2][1]) / 100

        normal = numpy.cross(numpy.subtract(tri[0], tri[2]), numpy.subtract(tri[0], tri[1]))
        normal = normal / numpy.linalg.norm(normal)
        slope = max(numpy.dot(normal, [0, 1, 0]), 0.0)

        if (elev < 0.05):
            if (slope < 0.9):
                return (0.1, 0.1, 0.1)
            else:
                return (0, 0.41, 0.58)
        elif (elev < 0.10):
            if (slope < 0.9):
                return (0.1, 0.1, 0.1)
            else:
                i = int((1 / 3) * (tri[0][0] + tri[1][0] + tri[2][0]))
                j = int((1 / 3) * (tri[0][2] + tri[1][2] + tri[2][2]))
                v = voronoi_diagram[i][j]
                return (0.0, v, 0.0)
        elif (elev < 0.30):
            i = int((1 / 3) * (tri[0][0] + tri[1][0] + tri[2][0]))
            j = int((1 / 3) * (tri[0][2] + tri[1][2] + tri[2][2]))
            v = voronoi_diagram[i][j]
            if (slope < 0.5):
                return (0.1, 0.1, 0.1)
            else:
                return (0.5 * 87 / 256 * v, 0.5 * 59 / 256 * v, 0.5 * 12 / 256 * v)
        else:
            if (slope < 0.5):
                return (0.1, 0.1, 0.1)
            else:
                return (1.0, 1.0, 1.0)

    terrain_pts = []
    for i in range(width):
        for j in range(width):
            p0 = get_point_on_height_map(i, j)
            p1 = get_point_on_height_map(i - 1, j)
            p2 = get_point_on_height_map(i + 1, j)
            p3 = get_point_on_height_map(i, j - 1)
            p4 = get_point_on_height_map(i, j + 1)

            height_delta = 0.2 * (abs(p1 - p0) + abs(p2 - p0) + abs(p3 - p0) + abs(p4 - p0))

            if (random.random() < 0.05):
                terrain_pts.append((i, j)) 
            else:
                if (height_delta > 0.005 and random.random() > 0.8):
                    terrain_pts.append((i, j)) 

    out_file = open('terrain_mesh.bin', 'w')
    terrain_tris = triangle.delaunay(terrain_pts)

    out_file.write(str(width) + '\n')

    for i in range(width):
        for j in range(width):
            elev = 100 * height_map[i][j]
            out_file.write(str(elev) + '\n')

    out_file.write(str(len(terrain_tris)) + '\n')
    for tri in terrain_tris:
        t = []
        for i in range(3):
            pt = terrain_pts[tri[i]]
            elev = 100 * height_map[pt[0]][pt[1]]
            t.append([pt[0], elev, pt[1]])
        color = get_triangle_color(t)
        for pt in t:
            out_file.write(str(pt[0]) + ' ' + str(pt[1]) + ' ' + str(pt[2]) + '\n')
            out_file.write(str(color[0]) + ' ' + str(color[1]) + ' ' + str(color[2]) + '\n')

    if (save_image):
        img = Image.new('RGB', (width, width))
        for i in range(width):
            for j in range(width):
                img.putpixel((i, j), (255, 255, 255))
        for pt in terrain_pts:
            img.putpixel(pt, (0, 0, 0))
        img.save(file_name)

    return terrain_pts

def create_voronoi_diagram(num_cells):
    diagram = []

    nx = []
    ny = []
    nv = []

    for i in range(num_cells):
        nx.append(random.randrange(width))
        ny.append(random.randrange(width))
        nv.append(random.random() * 0.6 + 0.4)

    for y in range(width):
        diagram.append([])
        for x in range(width):
            dmin = math.hypot(width - 1, width - 1)
            j = -1
            for i in range(num_cells):
                d = math.hypot(nx[i] - x, ny[i] - y)
                if d < dmin:
                    dmin = d
                    j = i
            diagram[y].append(nv[j])

    return diagram

def create_bezier_path(save_image = False):
    points = []
    for i in range(10):
        x = (width - 1) * (i / 9)
        y = width * random.random()
        points.append(numpy.array([x, y]))

    coeff = [1, 9, 36, 84, 126, 126, 84, 36, 9, 1]
    path = []

    num_samples = 10000
    for i in range(num_samples + 1):
        t = i / num_samples
        p = numpy.array([0, 0])
        for j in range(10):
            p = p + pow(1 - t, j) * pow(t, 9 - j) * coeff[j] * points[j]
        path.append(p)

    num_rectangles = 1
    rectangle_width = 20
    rectangles = []
    for i in range(num_rectangles):
        rectangles.append([])

    for j in range(num_rectangles):
        for i in range(len(path) - 1):
            d = path[i + 1] - path[i]
            d[0], d[1] = -d[1], d[0]
            d = d / numpy.linalg.norm(d)

            x1 = int((path[i] + (rectangle_width + j) * d)[0])
            y1 = int((path[i] + (rectangle_width + j) * d)[1])

            x2 = int((path[i + 1] + (rectangle_width + j) * d)[0])
            y2 = int((path[i + 1] + (rectangle_width + j) * d)[1])

            x3 = int((path[i + 1] - (rectangle_width + j) * d)[0])
            y3 = int((path[i + 1] - (rectangle_width + j) * d)[1])

            x4 = int((path[i] - (rectangle_width + j) * d)[0])
            y4 = int((path[i] - (rectangle_width + j) * d)[1])

            val = 255 - int(255 * j / num_rectangles);

            rectangles[j].append(([x1, y1, x2, y2, x3, y3, x4, y4, x1, y1], (val, val, val)))

    for i in range(len(path) - 2):
        r1 = rectangles[0][i]
        r2 = rectangles[0][i + 1]

        val = 255

        x1 = r1[0][2]  
        y1 = r1[0][3]  

        x2 = r1[0][4]  
        y2 = r1[0][5]  

        x3 = r2[0][6]  
        y3 = r2[0][7]  

        x4 = r2[0][0]  
        y4 = r2[0][1]  

        rectangles[0].append(([x1, y1, x2, y2, x3, y3, x4, y4, x1, y1], (val, val, val)))

    img = Image.new('RGB', (width, width))
    draw = ImageDraw.Draw(img)
    for j in reversed(range(num_rectangles)):
        for rect in rectangles[j]:
            draw.polygon(rect[0], fill=rect[1])
    del draw

    if (save_image):
        img.save('rectangles.png')

    pixels = img.load()

    bezier_path = []
    for i in range(width):
        bezier_path.append([])
        for j in range(width):
            bezier_path[i].append(pixels[i, j][0])

    return bezier_path

#create_bezier_path(True)
create_terrain_mesh(True)
