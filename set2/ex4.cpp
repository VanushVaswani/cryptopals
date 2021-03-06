#include <iostream>
#include <map>
#include <ex.h>

int determine_block_size()
{
	vector<uint8_t> ciphertext;

	vector<uint8_t> plaintext{'A'};

	aes_encrypt_ecb(plaintext, (unsigned char *) "x", ciphertext);

	auto result = ciphertext.size();
	while (true) {
		plaintext.push_back('A');
		aes_encrypt_ecb(plaintext, (unsigned char *) "x", ciphertext);

		if (ciphertext.size() != result) {
			result = ciphertext.size() - result;
			break;
		}
	}

	return result;
}

int decrypt_block(unsigned bs, vector<uint8_t> key,
	vector<uint8_t>& decoded_bytes, vector<uint8_t>& ciphertext,
	vector<uint8_t>& input_data)
{
	if (input_data.size() == 0) {
		return false;
	}
	// Decrypt single block
	for(unsigned i = 0; i < min(bs, (unsigned)input_data.size()); i++) {
		map<vector<uint8_t>, uint8_t> dict;
		// Create dictionary
		for(int j = 0; j < 256; j++) {
			vector<uint8_t> pfx(bs - 1 - i, 'A');
			vector<uint8_t> cipher;
			pfx.insert(pfx.end(), decoded_bytes.begin(),
				decoded_bytes.end());
			pfx.push_back((uint8_t)j);
			assert(pfx.size() == (unsigned)bs);
			aes_encrypt_ecb(pfx, key.data(), cipher);
			cipher.resize(bs);
			dict[cipher] = (uint8_t) j;
		}

		vector<uint8_t> prefix(bs - 1 - i, 'A');
		prefix.insert(prefix.end(), decoded_bytes.begin(),
			decoded_bytes.end());
		assert(prefix.size() == (unsigned) bs - 1);

		prefix.insert(prefix.end(), input_data.begin()
			+ i, input_data.end());

		aes_encrypt_ecb(prefix, key.data(), ciphertext);
		vector<uint8_t> tmp(ciphertext.begin(),
			ciphertext.begin() + 16);
		if (dict.find(tmp) != dict.end()) {
			decoded_bytes.push_back(dict[tmp]);
		} else {
			assert(!"not found");
		}
	}
	return true;
}
