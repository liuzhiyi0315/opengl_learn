class Quad {
    public:
        Quad();
        virtual ~Quad();
        void bind();
        void render();
    private:
        unsigned int quadVAO;
        unsigned int quadVBO[2];
};