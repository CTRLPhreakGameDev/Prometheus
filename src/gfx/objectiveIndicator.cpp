#include "gfx/objectiveIndicator.hpp"
#include <cmath>

void ObjectiveIndicator::Draw(Vector2 playerScreenPos, Vector2 objectiveWorldPos, const Camera2D& camera, int renderW, int renderH) const
{
    Vector2 objScreen = GetWorldToScreen2D(objectiveWorldPos, camera);
    float margin = 24.0f;
    bool onScreen = objScreen.x >= margin && objScreen.x <= renderW - margin && objScreen.y >= margin && objScreen.y <= renderH - margin;

    if (onScreen)
        return;

    Vector2 center = { renderW / 2.0f, renderH / 2 };
    float dx = objScreen.x - center.x;
    float dy = objScreen.y - center.y;
    float angle = atan2f(dy,dx);

    float edgeX = center.x + cosf(angle) * (center.x - margin);
    float edgeY = center.y + sinf(angle) * (center.y - margin);

    edgeX = Clamp(edgeX, margin, renderW - margin);
    edgeY = Clamp(edgeY, margin, renderH - margin);

    float arrowSize = 10.0f;
    Vector2 tip = { edgeX + cosf(angle) * arrowSize, edgeY + sinf(angle) * arrowSize };
    Vector2 left = { edgeX + cosf(angle + 2.4f) * arrowSize, edgeY + sinf(angle + 2.4f) * arrowSize };
    Vector2 right = { edgeX + cosf(angle - 2.4f) * arrowSize, edgeY + sinf(angle - 2.4f) * arrowSize };
    DrawTriangle(tip, left, right, YELLOW);
    DrawTriangleLines(tip, left, right, ORANGE);

    float worldDx = objectiveWorldPos.x - camera.target.x;
    float worldDy = objectiveWorldPos.y - camera.target.y;
    float dist = sqrtf(worldDx * worldDx + worldDy * worldDy);

    const char* label = TextFormat("%.1fm", dist);
    DrawText(label, (int)edgeX - MeasureText(label, 10) / 2, (int)edgeY + 14, 10, YELLOW);

}