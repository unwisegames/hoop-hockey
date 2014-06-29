# -*- coding: utf-8 -*-

image('digifont.xcf')
scale(20)

sprites(glyphs=font(ur'''
    1234567890''',
    combine=[('"', 2)],
    baselines=guides()['horizontal']
))
