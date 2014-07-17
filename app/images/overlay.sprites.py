image('overlay.xcf')
grid(12, 12)
scale(10)

sprites(
    fade=floodfill((1.5, 2), scale=90, origin='center'),
    window=floodfill((9, 5), scale=20, origin='centroid'),
    box=floodfill((2.6, 4), origin='centroid'),
    score=floodfill((4.5, 1))
)
