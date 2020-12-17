#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <random>
#include <algorithm>
#include <cmath>
#include <utility>
#include <chrono>
#include <fstream>

#include "hashset/ChainHashSet.hpp"
#include "hashset/LinearProbeHashSet.hpp"
#include "hashset/QuadraticProbeHashSet.hpp"
#include "hashset/DoubleHashingHashSet.hpp"
#include "hashset/CuckooHashSet.hpp"

#include "hash/md5.hpp"
#include "hash/sha256.hpp"
#include "hash/murmur3.hpp"

using namespace std;
using namespace hashset;

default_random_engine rg;

vector<string> read_lines(istream& in) {
    vector<string> lines;

    for (   
            string line;
            in >> line;
            lines.emplace_back(std::move(line))
        );

    return lines;
}

pair<vector<string>, vector<string>> gen_data(
    const vector<string>& source, const size_t n
) {
    vector<string> data;
    sample(
        source.begin(), source.end(), 
        back_inserter(data), n * 3 / 2, rg
    );

    const size_t size = data.size();

    vector<string> questions;
    copy(data.begin(), data.begin() + size / 2, back_inserter(questions));
    copy(data.begin() + size * 2 / 3, data.end(), back_inserter(questions));
    shuffle(questions.begin(), questions.end(), rg);

    data.resize(size * 2 / 3);

    return {data, questions};
}

template<typename T, typename Set>
void test(const vector<T>& source, size_t size, ostream& res) {
    res << "size\tmean_time" << endl;
    
    discrete_distribution choice{0.1, 0.9};

    for (size_t n = 10; n < size; n = n * 3 / 2) {
        chrono::nanoseconds total(0);
        
        const size_t rounds = 10;
        for (size_t round = 0; round < rounds; ++round) {
            const auto& [sample, quests] = gen_data(source, n);

            Set set;

            auto start = chrono::high_resolution_clock::now();
            for (const auto& elem: sample)
                set.insert(elem);
            
            for (const auto& elem: quests)
                if (choice(rg)) set.find(elem);
                else set.remove(elem);
            auto finish = chrono::high_resolution_clock::now();

            total += chrono::duration_cast<chrono::nanoseconds>(finish - start);
        }

        const size_t mean = total.count() / (rounds * 2 * n);
        res << n << "\t" << mean << endl;
    }
}

int main(int argc, char* argv[]) {
    vector<string> words = read_lines(cin);

    vector<string> data;
    const size_t test_max = 1000000;
    while (data.size() < test_max * 3 / 2) 
        data.insert(data.end(), words.begin(), words.end());
    
    words.clear();

    bool chain = false;
    bool linear = false;
    bool quadratic = false;
    bool doublehashing = false;
    bool cuckoo = false;

    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "chain")
            chain = true;
        else if (string(argv[i]) == "linear")
            linear = true;
        else if (string(argv[i]) == "quadratic")
            quadratic = true;
        else if (string(argv[i]) == "doublehashing")
            doublehashing = true;
        else if (string(argv[i]) == "cuckoo")
            cuckoo = true;
    }

    string pref = "data/";

    // chain
    if (chain) {
        cout << "Testing chain..." << endl;

        ofstream chain_md5(pref + "chain_md5.csv", ofstream::out | ofstream::trunc);
        test<string, ChainHashSet<string, md5hash<string>>>(data, test_max, chain_md5);
        chain_md5.close();

        ofstream chain_sha256(pref + "chain_sha256.csv", ofstream::out | ofstream::trunc);
        test<string, ChainHashSet<string, sha256hash<string>>>(data, test_max, chain_sha256);
        chain_sha256.close();

        ofstream chain_murmur(pref + "chain_murmur.csv", ofstream::out | ofstream::trunc);
        test<string, ChainHashSet<string, murmur3hash<string, 123>>>(data, test_max, chain_murmur);
        chain_murmur.close();
    }

    // linear
    if (linear) {
        cout << "Testing linear..." << endl;

        ofstream linear_md5(pref + "linear_md5.csv", ofstream::out | ofstream::trunc);
        test<string, LinearProbeHashSet<string, md5hash<string>>>(data, test_max, linear_md5);
        linear_md5.close();

        ofstream linear_sha256(pref + "linear_sha256.csv", ofstream::out | ofstream::trunc);
        test<string, LinearProbeHashSet<string, sha256hash<string>>>(data, test_max, linear_sha256);
        linear_sha256.close();

        ofstream linear_murmur(pref + "linear_murmur.csv", ofstream::out | ofstream::trunc);
        test<string, LinearProbeHashSet<string, murmur3hash<string, 123>>>(data, test_max, linear_murmur);
        linear_murmur.close();
    }

    // quadratic
    if (quadratic) {
        cout << "Testing quadratic..." << endl;

        ofstream quadratic_md5(pref + "quadratic_md5.csv", ofstream::out | ofstream::trunc);
        test<string, QuadraticProbeHashSet<string, md5hash<string>>>(data, test_max, quadratic_md5);
        quadratic_md5.close();

        ofstream quadratic_sha256(pref + "quadratic_sha256.csv", ofstream::out | ofstream::trunc);
        test<string, QuadraticProbeHashSet<string, sha256hash<string>>>(data, test_max, quadratic_sha256);
        quadratic_sha256.close();

        ofstream quadratic_murmur(pref + "quadratic_murmur.csv", ofstream::out | ofstream::trunc);
        test<string, QuadraticProbeHashSet<string, murmur3hash<string, 123>>>(data, test_max, quadratic_murmur);
        quadratic_murmur.close();
    }

    // double
    if (doublehashing) {
        cout << "Testing doublehashing..." << endl;

        ofstream double_md5(pref + "double_md5.csv", ofstream::out | ofstream::trunc);
        test<string, DoubleHashingHashSet<string, md5hash<string>, std::hash<string>>>(data, test_max, double_md5);
        double_md5.close();

        ofstream double_sha256(pref + "double_sha256.csv", ofstream::out | ofstream::trunc);
        test<string, DoubleHashingHashSet<string, sha256hash<string>, std::hash<string>>>(data, test_max, double_sha256);
        double_sha256.close();

        ofstream double_murmur(pref + "double_murmur.csv", ofstream::out | ofstream::trunc);
        test<string, DoubleHashingHashSet<string, murmur3hash<string, 123>, std::hash<string>>>(data, test_max, double_murmur);
        double_murmur.close();
    }

    // cuckoo
    if (cuckoo) {
        cout << "Testing cuckoo..." << endl;

        ofstream cuckoo_md5_sha256(pref + "cuckoo_md5_sha256.csv", ofstream::out | ofstream::trunc);
        test<string, CuckooHashSet<string, md5hash<string>, sha256hash<string>>>(data, test_max, cuckoo_md5_sha256);
        cuckoo_md5_sha256.close();

        ofstream cuckoo_md5_murmur(pref + "cuckoo_md5_murmur.csv", ofstream::out | ofstream::trunc);
        test<string, CuckooHashSet<string, md5hash<string>, murmur3hash<string, 123>>>(data, test_max, cuckoo_md5_murmur);
        cuckoo_md5_murmur.close();

        ofstream cuckoo_sha256_murmur(pref + "cuckoo_sha256_murmur.csv", ofstream::out | ofstream::trunc);
        test<string, CuckooHashSet<string, sha256hash<string>, murmur3hash<string, 123>>>(data, test_max, cuckoo_sha256_murmur);
        cuckoo_sha256_murmur.close();
    }
}
