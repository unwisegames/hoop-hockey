image('buttons.xcf')
grid(12, 12)
scale(10)

sprites(
    arcade=[
        floodfill((3, 1), scale=14, origin='centroid'),
        floodfill((3, 4), scale=14, origin='centroid')
    ],
    buzzer=[
        floodfill((9, 1), scale=14, origin='centroid'),
        floodfill((9, 4), scale=14, origin='centroid')
    ],
    restart=[
        floodfill((2, 6), scale=16, origin='centroid'),
        floodfill((2, 8), scale=16, origin='centroid')
    ],
    exit=[
        floodfill((7, 6), scale=16, origin='centroid'),
        floodfill((7, 8), scale=16, origin='centroid')
    ],
)
