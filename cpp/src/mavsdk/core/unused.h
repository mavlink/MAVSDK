#pragma once

template<typename... Args> void UNUSED(Args&&... args)
{
    (void)(sizeof...(args));
}