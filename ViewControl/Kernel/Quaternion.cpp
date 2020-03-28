
#include "stdafx.h"
#include "Quaternion.h"

const Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);
const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
int Quaternion::ms_iNext[3] = { 1, 2, 0 };