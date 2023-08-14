#ifndef SHADOW_H
#define	SHADOW_H
#include"Lights.h"

void drawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);
void generateLightFBOAndTex(GLuint& lightFBO, GLuint& lightFBOTex);
std::vector<glm::mat4> getLightSpaceMatrices();
std::vector<glm::vec4> getFrustrumCornersWorldSpace(const glm::mat4& projView);

namespace Shadow_values {
	extern std::vector<GLfloat> shadowCascadeLevels;
}
#endif // !SHADOW_H

