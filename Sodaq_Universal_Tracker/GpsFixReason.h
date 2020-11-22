// Used to indicate *why* the GPS fix was indicated. This can be useful to determine if this was movement or another reason.
enum GpsFixReason : uint8_t {
    UNKNOWN = 0,
    SETUP = 1,
    DEFAULT_FIX_INTERVAL = 2, 
    ALTERNATIVE_FIX_INTERVAL = 3,
    ON_THE_MOVE = 4
};