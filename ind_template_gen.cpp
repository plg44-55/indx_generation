#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include <iterator>
#include <bitset>
#include <functional>

// this is general case
// struct Record
// {
//     unsigned keyA;
//     std::string keyB;
//     double keyC;
//     unsigned long value;
// };

// this is particular case

using SID = unsigned;

struct Test
{
    SID a_nmb;
    SID b_nmb;
};

struct Record
{
    SID dir;
    SID pbx_a;
    SID oper_a;
    SID pbx_b;
    SID oper_b;
    SID tg;
    SID zone_a;
    SID pref_a;
    SID cg_a;
    SID zone_b;
    SID pref_b;
    SID cg_b;
    Test test;
    double value;
};

constexpr unsigned	DIR    = 0;
constexpr unsigned	PBX_A  = 1;
constexpr unsigned	OPER_A = 2;
constexpr unsigned	PBX_B  = 3;
constexpr unsigned	OPER_B = 4;
constexpr unsigned	TG     = 5;
constexpr unsigned	ZONE_A = 6;
constexpr unsigned	PREF_A = 7;
constexpr unsigned	CG_A   = 8;
constexpr unsigned	ZONE_B = 9;
constexpr unsigned	PREF_B = 10;
constexpr unsigned	CG_B   = 11;
constexpr unsigned 	TEST_A_NMB = 12;

// dir
// dir, pbx
// dir, pbx, nbr
// pbx
// pbx, nbr
// nbr

constexpr unsigned FIELDS_COUNT = 13;
//using RecordKey = std::tuple<SID, SID, SID>;
using RecordKey = std::array<SID, FIELDS_COUNT>;
using RecordsMap = std::multimap<RecordKey, Record>;

//using PtrToField = SID Record::*;
using PtrToField = std::function<SID (Record const&)>;
using FieldsOfKey = std::array<PtrToField, FIELDS_COUNT>;
using KeyCombinations = std::vector<FieldsOfKey>;

KeyCombinations all_subsets_of_field_index;
PtrToField tt = [](Record const& r) { return r.test.a_nmb; };

const FieldsOfKey fields
{
    &Record::dir,		// 0
    &Record::pbx_a,		// 1
    &Record::oper_a,		// 2
    &Record::pbx_b,		// 3
    &Record::oper_b,		// 4
    &Record::tg,		// 5
    &Record::zone_a,		// 6
    &Record::pref_a,		// 7
    &Record::cg_a,		// 8
    &Record::zone_b,		// 9
    &Record::pref_b,		// 10
    &Record::cg_b,		// 11
    tt
};

struct Query
{
    std::vector<SID> l1;
    std::vector<SID> l2;
    std::vector<SID> l3;
};

constexpr unsigned COMBINATIONS_COUNT = 1 << FIELDS_COUNT;

std::vector<Record>
readRecords()
{
    return
	{
	    { 1, 20, 30, 22, 32, 43, 51, 62, 73, 51, 63, 74, {200, 0}, 1.0 },
	    { 4, 25, 36, 22, 32, 43, 51, 62, 73, 51, 63, 74, {200, 0}, 1.0 },
	    { 7, 28, 39, 22, 32, 43, 51, 62, 73, 51, 63, 74, {200, 0}, 1.0 },
	    { 1, 25, 36, 22, 32, 43, 51, 62, 73, 51, 63, 74, {200, 0}, 11.0 },
	    { 1, 21, 30, 22, 32, 43, 51, 62, 73, 51, 63, 74, {200, 0}, 1.0 },
	    { 1, 21, 30, 22, 32, 43, 51, 62, 73, 51, 63, 74, {200, 0}, 100.0 }
	};
}

std::ostream&
operator << (std::ostream& o, const RecordKey k)
{
    std::copy(k.begin(), k.end(), std::ostream_iterator<SID>(o, ", "));
    return o;
}

std::ostream&
operator << (std::ostream& o, const Record k)
{
    o << k.dir << " " << k.pbx_a << " " << k.oper_a << " "
      << k.pbx_b << " " << k.oper_b << " " << k.tg << " "
      << k.zone_a << " " << k.pref_a << " " << k.cg_a << " "
      << k.zone_b << " " << k.pref_b << " " << k.cg_b << " {"
      << k.test.a_nmb << ", " << k.test.b_nmb << "}"
      << " = " << k.value;
    return o;
}

constexpr unsigned QUERY_KEYS_COUNT = 2;

int
main()
{
    for( auto i = 1; i < COMBINATIONS_COUNT; ++i )
    {
	std::bitset<FIELDS_COUNT> flds(i);
	if( flds.count() > QUERY_KEYS_COUNT )
	    continue;

	FieldsOfKey fld_combo;
	for( auto j = 0; j < FIELDS_COUNT; ++j )
	    fld_combo[j] = flds[j] ? fields[j] : nullptr;

	if( flds[CG_A] && flds[CG_B] && flds.count() == 2 )
	    continue;

	all_subsets_of_field_index.push_back(fld_combo);
    }
    std::cout << "Here " << all_subsets_of_field_index.size() << " combinations\n";

    std::vector<Record> records = readRecords();
    RecordsMap n_em;
    for( auto& r : records )
    {
	for( auto& fld_combo : all_subsets_of_field_index )
	{
	    RecordKey key;
	    std::transform(fld_combo.begin(), fld_combo.end(), key.begin(),
			   [&r](const PtrToField& f) { return (f != nullptr) ? f(r) : 0; });

	    n_em.insert(std::pair<RecordKey, const Record&>( {key, r} ));
	}
    }
    std::cout << "Here " << n_em.size() << " keys\n";
    std::cout << " 1 - dir, 20 - pbx, 30 - oper, 40 - tg, 50 - zn, 60 - pref, 70 - cg\n\n";
    for( auto& r : n_em )
	std::cout << r.first << "   --->   " << r.second << "\n";
}
