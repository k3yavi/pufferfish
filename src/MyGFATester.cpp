#include "FastxParser.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <iterator>
#include <type_traits>
#include "sdsl/int_vector.hpp"
#include "jellyfish/mer_dna.hpp"
#include "string_view.hpp"
//#include "PufferFS.hpp"
#include "BooPHF.h"
//#include "OurGFAReader.hpp"
#include "popl.hpp"
#include "ScopedTimer.hpp"
#include "sdsl/rank_support.hpp"
#include "sdsl/select_support.hpp"
#include "sparsepp/spp.h"
#include "Util.hpp"
/*char complement(char& c){
    switch(c){
        case 'A': c = 'T';
                  return c;
        case 'T': c = 'A';
                  return c;
        case 'C': c = 'G';
                  return c;
        case 'G': c = 'C';
                  return c;

    }
}

std::string revcomp(std::string s){
    int n = s.size();
    int halfLength = s.size() / 2;
    for (int i=0; i<halfLength; i++)
    {
        char temp = complement(s[i]);
        s[i] = complement(s[n-1-i]);
        s[n-1-i] = temp;
    }
    if(s.size()%2 != 0){
    	s[halfLength] = complement(s[halfLength]);
    }
    return s;
}

bool is_number(const std::string& s) {
	return !s.empty() && std::find_if(s.begin(),
						s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}


std::vector<std::pair<std::string, bool> > explode(const stx::string_view str, const char& ch) {
				std::string next;
				std::vector< std::pair<std::string, bool> > result;
				// For each character in the string
				for (auto it = str.begin(); it != str.end(); it++) {
					// If we've hit the terminal character
					if (*it == '+' or *it == '-') {
						bool orientation = true;
						// If we have some characters accumulated
						// Add them to the result vector
						if (!next.empty()) {
							if (*it == '-') {
								orientation = false;
							}
							result.emplace_back(next, orientation);
							next.clear();
						}
					}
					else if (*it != ch) {
						// Accumulate the next character into the sequence
						next += *it;
					}
				}
				if (!next.empty())
					result.emplace_back(next, true); //this case shouldn't even happen
				return result;
			}

std::vector<stx::string_view> split(stx::string_view str, char delims) {
	std::vector<stx::string_view> ret;

	stx::string_view::size_type start = 0;
	auto pos = str.find_first_of(delims, start);
	while (pos != stx::string_view::npos) {
		if (pos != start) {
			ret.push_back(str.substr(start, pos - start));
		}
		start = pos + 1;
		pos = str.find_first_of(delims, start);
	}
	if (start < str.length()) {
		ret.push_back(str.substr(start, str.length() - start));
	}
	return ret;
}
*/

int main(int argc, char* argv[]){
	std::vector<std::string> read_file = {argv[1]} ;
	std::string gfa_file = argv[2] ;

	std::cerr << "\n fasta file " << argv[1] << "\n" ;
	std::cerr << "\n gfa file " << argv[2] << "\n" ;
	//std::string outfile = "contigs.fa" ;

	spp::sparse_hash_map<std::string, std::string> fastaMap ;


	 std::cout << "\n starting .. \n" ;
	{
		fastx_parser::FastxParser<fastx_parser::ReadSeq> parser(read_file, 1, 1);
		parser.start();
		 auto rg = parser.getReadGroup();
		 uint32_t rn = 0;
		 while (parser.refill(rg)) {
				 // Here, rg will contain a chunk of read pairs
				 // we can process.
				 for (auto& rp : rg) {
					 if (rn % 10000 == 0) {
						 std::cerr << "rn : " << rn << "\n";
					 }
					 ++rn;

					 auto& r1 = rp.seq ;
					 fastaMap[rp.name] = r1 ;
				 }
		 }
	}

	std::cerr << "\n fasta file contains " <<fastaMap.size() << " \n" ;

	std::ifstream file(gfa_file) ;


		std::string ln;
		std::string tag, id, value;
		size_t contig_cnt{0};
		size_t ref_cnt{0};
		spp::sparse_hash_map<std::string, bool> touchedSegment ;
		spp::sparse_hash_map<std::string, std::string> contigid2seq ;
		spp::sparse_hash_map<std::string, std::string> reconstructedTr ;

		while(std::getline(file, ln)) {
				char firstC = ln[0];
				if (firstC != 'S' and firstC != 'P') continue;
				stx::string_view lnview(ln);
				std::vector<stx::string_view> splited = util::split(lnview, '\t');
				tag = splited[0].to_string();
				id = splited[1].to_string();
				value = splited[2].to_string();
				// A segment line
				if (tag == "S") {
					if (util::is_number(id)) {
						contigid2seq[id] = value;
						//touchedSegment[id] = false ;
					}
					contig_cnt++;
				}


				// A path line
				if (tag == "P") {
					auto pvalue = splited[2];
					std::vector<std::pair<std::string, bool> > contigVec = util::explode(pvalue, ',');
					// parse value and add all conitgs to contigVec
					//if(reconstructedTr[id] != "") continue ;
					reconstructedTr[id] = "";
					int i = 0;
					if(id == "ENST00000393481.6|ENSG00000135269.17|OTTHUMG00000023092.9|OTTHUMT00000137414.1|TES-002|TES|2763|UTR5:1-223|CDS:224-1462|UTR3:1463-2763|"){
						//std::cerr << " number of contigs " << contigVec.size() << "\n" ;
					}
					for(auto core : contigVec){
						auto contig_id = core.first ;
						auto ore = core.second ;
						std::string added ;
						if (i != contigVec.size()-1){
							if(!ore){
								added =  util::revcomp(contigid2seq[contig_id]) ;
							}else{
								added = contigid2seq[contig_id] ;
							}
							//contigid2seq[contig_id].erase(contigid2seq[contig_id].size()-31+1,31) ;
							if(added.size() > 31){
								added.erase(added.size()-31,31) ;
								reconstructedTr[id] += added ;
							}
						}else{
							if(!ore){
								added =  util::revcomp(contigid2seq[contig_id]) ;
							}else{
								added = contigid2seq[contig_id] ;
							}
							reconstructedTr[id] += added ;
						}
						i++ ;
					}

					if(fastaMap[id] != reconstructedTr[id]){
						std::cerr << id << "\n" ;
						std::cerr << fastaMap[id] << " " << fastaMap[id].size() << "\n" ;
						std::cerr << reconstructedTr[id] << " " << reconstructedTr[id].size() << "\n" ;
						std::cerr << " number of contigs " << contigVec.size() << "\n" ;
						int j = 0;
						for(auto core : contigVec){
							auto contig_id = core.first ;
							auto ore = core.second ;
							std::string added ;
							if (j != contigVec.size()-1){
								if(!ore){
									added =  util::revcomp(contigid2seq[contig_id]) ;
								}else{
									added = contigid2seq[contig_id] ;
								}
								//contigid2seq[contig_id].erase(contigid2seq[contig_id].size()-31+1,31) ;
								if(added.size() > 31){
									added.erase(added.size()-31,31) ;
									reconstructedTr[id] += added ;
									std::cerr << contig_id << " " << added << "\n" ;
									//std::cerr << added << "\n" ;
								}
									//std::cerr << contig_id << " " << added << "\n" ;
							}else{
								if(!ore){
									added =  util::revcomp(contigid2seq[contig_id]) ;
								}else{
									added = contigid2seq[contig_id] ;
								}
								//reconstructedTr[id] += added ;
								std::cerr << contig_id << " " << added << "\n" ;
							}
							j++ ;
						}


						//std::exit(1) ;
					}

				}
		}


		int found = 0;
		int notFound = 0;
		for(auto& kv: fastaMap){
			if(kv.second == reconstructedTr[kv.first]){
				found++ ;
			}else{
				std::cerr << "tid " << kv.first << "\n" ;
				std::cerr << "true seq " << kv.second << "\n" ;
				std::cerr << "our seq " << reconstructedTr[kv.first] << "\n" ;
				notFound++ ;
				std::exit(1) ;
			}
		}

		std::cerr << "\n Found " << found << " Not Found " << notFound << "\n" ;


	return 0 ;

}
