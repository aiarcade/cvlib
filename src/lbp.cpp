
#include "lbp.h"

unsigned int Lbp::getValueBilinear(unsigned int *img, float x, float y, int width)
{
    unsigned int pt[4];
    unsigned int *p = img + (int)x + (int)y * width;
    double x1, x2;
    double y1, y2;

    pt[0] = *p;
    pt[1] = *(p+1);
    pt[2] = *(p+width);
    pt[3] = *(p+width+1);

    x1 = floorf(x);
    x2 = x1 + 1;
    y1 = floorf(y);
    y2 = y1 + 1;

    return (unsigned int)(pt[0] * (x2 - x) * (y2 - y) +
               pt[1] * (x - x1) * (y2 - y) +
               pt[2] * (x2 - x) * (y - y1) +
               pt[3] * (x - x1) * (y - y1));
}

void Lbp::getInterpolatedIntegralValue(LbpRect *r, unsigned int *img, int x, int y, int width, float scale, unsigned int *p)
{

  unsigned int i[16];
    float fx, fy;

    fx = (float)x + r->x * scale;
    fy = (float)y + r->y * scale;

    i[0] = getValueBilinear(img, fx,                  fy, width);
    i[1] = getValueBilinear(img, fx + r->w*scale,     fy, width);
    i[2] = getValueBilinear(img, fx + r->w*scale * 2, fy, width);
    i[3] = getValueBilinear(img, fx + r->w*scale * 3, fy, width);

    i[4] = getValueBilinear(img, fx,                  fy + r->h*scale, width);
    i[5] = getValueBilinear(img, fx + r->w*scale,     fy + r->h*scale, width);
    i[6] = getValueBilinear(img, fx + r->w*scale * 2, fy + r->h*scale, width);
    i[7] = getValueBilinear(img, fx + r->w*scale * 3, fy + r->h*scale, width);

    i[8]  = getValueBilinear(img, fx,                  fy + r->h*scale * 2, width);
    i[9]  = getValueBilinear(img, fx + r->w*scale,     fy + r->h*scale * 2, width);
    i[10] = getValueBilinear(img, fx + r->w*scale * 2, fy + r->h*scale * 2, width);
    i[11] = getValueBilinear(img, fx + r->w*scale * 3, fy + r->h*scale * 2, width);

    i[12] = getValueBilinear(img, fx,                  fy + r->h*scale * 3, width);
    i[13] = getValueBilinear(img, fx + r->w*scale,     fy + r->h*scale * 3, width);
    i[14] = getValueBilinear(img, fx + r->w*scale * 2, fy + r->h*scale * 3, width);
    i[15] = getValueBilinear(img, fx + r->w*scale * 3, fy + r->h*scale * 3, width);

    p[0] = (i[0] - i[1] - i[4] + i[5]);
    p[1] = (i[1] - i[2] - i[5] + i[6]);
    p[2] = (i[2] - i[3] - i[6] + i[7]);
    p[3] = (i[4] - i[5] - i[8] + i[9]);
    p[4] = (i[5] - i[6] - i[9] + i[10]);
    p[5] = (i[6] - i[7] - i[10] + i[11]);
    p[6] = (i[8] - i[9] - i[12] + i[13]);
    p[7] = (i[9] - i[10] - i[13] + i[14]);
    p[8] = (i[10] - i[11] - i[14] + i[15]);

}
float Lbp::lbpClassify(LbpRect *r, WeakClassifier *c, unsigned int *img, int x, int y, int width, int height, float scale)
{
    /* 0 1 2
     * 3 4 5
     * 6 7 8
     */
    unsigned int p[9];
    unsigned char lbp_code;

    getInterpolatedIntegralValue(&r[c->rect_idx], img, x, y, width, scale, p);

    lbp_code = 0;
    if (p[0] >= p[4]) lbp_code |= 128;
    if (p[1] >= p[4]) lbp_code |= 64;
    if (p[2] >= p[4]) lbp_code |= 32;
    if (p[3] >= p[4]) lbp_code |= 1;
    if (p[5] >= p[4]) lbp_code |= 16;
    if (p[6] >= p[4]) lbp_code |= 2;
    if (p[7] >= p[4]) lbp_code |= 4;
    if (p[8] >= p[4]) lbp_code |= 8;

    if (c->lbpmap[lbp_code >> 5] & (1 << (lbp_code & 31))) {
        return c->neg;
    } else {
        return c->pos;
    }
}
