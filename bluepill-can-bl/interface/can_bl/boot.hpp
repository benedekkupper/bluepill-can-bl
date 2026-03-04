#pragma once

namespace can_bl
{
enum class request_source : unsigned short
{
    none = 0,
    app = 0xCAB0,
    loader = 0xB007,
};
request_source pop_update_request();
void set_update_request(request_source source = request_source::app);
} // namespace can_bl
