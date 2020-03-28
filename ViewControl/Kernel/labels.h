#pragma once

#include <string>

#include "Vector3.h"

void RenderRotatableString(Vector3 pos, Vector3 dir, void *font, const std::string &label);
void RenderBitmapString2D(float x, float y, void *font, const std::string &label);
void RenderBitmapString(float x, float y, float z, void *font, const std::string &label);
void RenderBitmapString(float x, float y, float z, const float *model_view_matrix, void *font, const std::string &label);
void RenderBitmapString(const float v[3], void *font, const std::string & label);
void RenderBitmapString(const float v[3], const float *model_view_matrix, void *font, const std::string &label);
void RenderBitmapString(const Vector3 &v, void *font, const std::string & label);
void RenderBitmapString(const Vector3 &v, const float *model_view_matrix, void *font, const std::string &label);

void RenderBitmapStringCentered(const Vector3 &v, void *font, const std::string & label);
