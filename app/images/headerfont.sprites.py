# -*- coding: utf-8 -*-

image('headerfont.xcf')
scale(10)

sprites(glyphs=font(ur'''
    0123456789
    ABCDEFGHIJ
    KLMNOPQRS
    TUVWXYZ!
    ''',
    baselines=guides()['horizontal']
))
