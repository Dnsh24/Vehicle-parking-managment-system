#include "auth.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{
constexpr int iterations = 210000;
constexpr std::size_t salt_size = 16;
constexpr std::size_t derived_key_size = 32;

std::string toHex(const unsigned char *bytes, std::size_t size)
{
    std::ostringstream output;
    output << std::hex << std::setfill('0');
    for (std::size_t index = 0; index < size; ++index)
    {
        output << std::setw(2) << static_cast<unsigned int>(bytes[index]);
    }
    return output.str();
}

std::vector<unsigned char> fromHex(const std::string &value)
{
    if (value.size() % 2 != 0)
    {
        throw std::invalid_argument("Invalid encoded password hash");
    }

    std::vector<unsigned char> bytes;
    bytes.reserve(value.size() / 2);
    for (std::size_t index = 0; index < value.size(); index += 2)
    {
        unsigned int byte = 0;
        std::istringstream parser(value.substr(index, 2));
        parser >> std::hex >> byte;
        if (parser.fail())
        {
            throw std::invalid_argument("Invalid encoded password hash");
        }
        bytes.push_back(static_cast<unsigned char>(byte));
    }
    return bytes;
}

std::vector<unsigned char> deriveKey(
    const std::string &password,
    const std::vector<unsigned char> &salt,
    int rounds)
{
    std::vector<unsigned char> key(derived_key_size);
    if (PKCS5_PBKDF2_HMAC(
            password.data(),
            static_cast<int>(password.size()),
            salt.data(),
            static_cast<int>(salt.size()),
            rounds,
            EVP_sha256(),
            static_cast<int>(key.size()),
            key.data()) != 1)
    {
        throw std::runtime_error("Password hashing failed");
    }
    return key;
}
}

namespace auth
{
std::string hashPassword(const std::string &password)
{
    std::vector<unsigned char> salt(salt_size);
    if (RAND_bytes(salt.data(), static_cast<int>(salt.size())) != 1)
    {
        throw std::runtime_error("Secure random generation failed");
    }

    const std::vector<unsigned char> key = deriveKey(password, salt, iterations);
    return "pbkdf2_sha256$" + std::to_string(iterations) + "$" +
           toHex(salt.data(), salt.size()) + "$" +
           toHex(key.data(), key.size());
}

bool verifyPassword(const std::string &password, const std::string &encoded_hash)
{
    try
    {
        std::stringstream parser(encoded_hash);
        std::string algorithm;
        std::string rounds_text;
        std::string salt_text;
        std::string key_text;
        if (!std::getline(parser, algorithm, '$') ||
            !std::getline(parser, rounds_text, '$') ||
            !std::getline(parser, salt_text, '$') ||
            !std::getline(parser, key_text, '$') ||
            algorithm != "pbkdf2_sha256")
        {
            return false;
        }

        const int rounds = std::stoi(rounds_text);
        const std::vector<unsigned char> salt = fromHex(salt_text);
        const std::vector<unsigned char> expected_key = fromHex(key_text);
        const std::vector<unsigned char> actual_key = deriveKey(password, salt, rounds);
        return expected_key.size() == actual_key.size() &&
               CRYPTO_memcmp(
                   expected_key.data(),
                   actual_key.data(),
                   actual_key.size()) == 0;
    }
    catch (const std::exception &)
    {
        return false;
    }
}

std::string createToken()
{
    std::vector<unsigned char> bytes(32);
    if (RAND_bytes(bytes.data(), static_cast<int>(bytes.size())) != 1)
    {
        throw std::runtime_error("Secure random generation failed");
    }
    return toHex(bytes.data(), bytes.size());
}

std::string hashToken(const std::string &token)
{
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_size = 0;
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (context == nullptr ||
        EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(context, token.data(), token.size()) != 1 ||
        EVP_DigestFinal_ex(context, digest, &digest_size) != 1)
    {
        EVP_MD_CTX_free(context);
        throw std::runtime_error("Token hashing failed");
    }
    EVP_MD_CTX_free(context);
    return toHex(digest, digest_size);
}
}
