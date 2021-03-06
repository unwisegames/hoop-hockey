image('atlas.xcf')
grid(12, 12)
scale(10)

def character(X, Y):
    return [floodfill((3 * X + x + 0.5, 3 * Y + y + 0.5), n=20)
            for (x, y) in [(0, 0), (1, 0), (2, 0), (1, 1), (2, 2)]]

def door(X, Y):
    return floodfill((X, Y), scale=8, origin='centroid')

sprites(
    platform=floodfill((4.5, 6.5), scale=20),
    scoreboard=[
        floodfill((8, 7), scale=20, origin='centroid'),
        floodfill((10, 1), scale=20, origin='centroid')
    ],
    hoop=[
        floodfill((4.5, 8), scale=11, origin='centroid'),
        floodfill((8.5, 8), scale=11, origin='centroid')
    ],
    threeline=floodfill((6, 10.5), scale=12.36, origin='centroid'),
    shotline=[
        floodfill((6, 11), scale=12.36, origin='centroid'),
        floodfill((6, 11.8), scale=12.36, origin='centroid'),
    ],
    door=[
        loop(
            (door(1, 5), 0.15),
            (door(3, 5), 0.15),
            (door(5, 5), 0.15),
            (door(7, 5), 0.15),
            (door(9, 5), 0.15),
            (door(11, 5), 0.5),
            (door(9, 5), 0.15),
            (door(7, 5), 0.15),
            (door(5, 5), 0.15),
            (door(3, 5), 0.15),
            (door(1, 5), 0.15),
        )
    ],
    ball=floodfill((11, 9), n=30),
    glow=floodfill((2.5, 1.5), scale=11, origin='center'),
    back=[
        floodfill((9, 3), scale=12, origin='center'),
        floodfill((10.5, 3), scale=12, origin='center')
    ],
)
