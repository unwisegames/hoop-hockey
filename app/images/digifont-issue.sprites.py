# -*- coding: utf-8 -*-

image('digifont-issue.xcf')
scale(20)

sprites(glyphs=font(ur'''
    1234567890A
    BCDEFGHIJKL
    MNOPQRSTUVW
    XYZ''',
    baselines=guides()['horizontal']
))
