#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

class DebugRenderer {
	public:
		static void render(unsigned char* image, const ObjectList& balls, const ObjectList& goals);

	private:
		static void renderBalls(ImageBuffer* img, const ObjectList& balls);
		static void renderGoals(ImageBuffer* img, const ObjectList& goals);
};

#endif