#ifndef COLOR_H
#define COLOR_H

enum Color
{
    RED = 1,
    GREEN = 2,
    BLUE = 3,
    UNDEF,
};

enum VertexState
{
    VERTEX_COLORABLE = 1,
    VERTEX_UNCOLORABLE = 2,
    VERTEX_STATE_NOT_DETERMINED,
};

#endif /* COLOR_H */
