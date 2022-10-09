#ifndef __CUBE_H__
#define __CUBE_H__


class Cube {
    public:
        Cube();
        virtual ~Cube();
        void Bind();
        void Render();
    private:
        unsigned int cubeVAO;
        unsigned int cubeVBO;
};
#endif // __CUBE_H__