image('overlay.xcf')
grid(12, 12)
scale(10)

sprites(
    fade=floodfill((1.5, 2), scale=999, origin='center'),
    window=floodfill((9, 5), scale=20, origin='center'),
    box=floodfill((2.6, 4), origin='centroid'),
    score=floodfill((4.5, 1)),
    #swish=floodfill((3, 6)),
    title=floodfill((3, 6), origin='centroid'),
    stat=floodfill((3, 8.5), scale=24, origin='centroid'),
    statsingle=floodfill((8, 8.5), scale=24, origin='centroid'),
)
