/*
    sha1.hpp - header of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Grabsch <vog@notjusthosting.com>
    Safety fixes
        -- Eugene Hopkinson <slowriot at voxelstorm dot com>
*/

#ifndef SHA1_HPP
#define SHA1_HPP


//#include <cstdint>
#include <iostream>
#include <string>


#ifndef uint32_t
typedef UINT32 uint32_t;
#endif
#ifndef uint8_t
typedef UINT8 uint8_t;
#endif
#ifndef uint64_t
typedef UINT64 uint64_t;
#endif


class SHA1
{
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    static std::string from_file(const std::string &filename);

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};


#endif /* SHA1_HPP */
