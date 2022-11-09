#pragma once

#include "display-renderer/IRenderTarget.hpp"

class DriverInterface : public IRenderTarget
{
public:
    void submitImage(const uint8_t *image, size_t length) override
    {
    }
};