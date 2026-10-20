#include "../src/auth/auth.h"

#include <cassert>

int main()
{
    const std::string password_hash = auth::hashPassword("correct horse battery staple");
    assert(password_hash != "correct horse battery staple");
    assert(auth::verifyPassword("correct horse battery staple", password_hash));
    assert(!auth::verifyPassword("wrong password", password_hash));

    const std::string token = auth::createToken();
    assert(token.size() == 64);
    assert(auth::hashToken(token) != token);
    return 0;
}
