#ifndef REF_SEQ_CONSTRUCTOR_HPP
#define REF_SEQ_CONSTRUCTOR_HPP


#include "Util.hpp"
#include "CanonicalKmer.hpp"

enum Task {
           SUCCESS,
           FAILURE
};

struct nextCompatibleStruct {
  util::ContigBlock cntg ;
  size_t tpos ;
  uint32_t cpos ;
  bool moveFw ;

  nextCompatibleStruct(util::ContigBlock cntgIn, size_t tposIn, uint32_t cposIn, bool mFw) : cntg(cntgIn), tpos(tposIn), cpos(cposIn), moveFw(mFw) {} 
} ;


template <typename PufferfishIndexT>
class RefSeqConstructor {

public:
  RefSeqConstructor(PufferfishIndexT* pfi);
  Task doBFS(size_t tid,
             size_t tpos,
             bool moveFw,
             util::ContigBlock& curContig,
             size_t startp,
             util::ContigBlock& endContig,
             size_t endp,
             uint32_t threshold,
             std::string& seq);
private:
  PufferfishIndexT* pfi_ ;
  size_t k ;



  size_t distance(size_t startp, size_t endp, bool moveFw); 
  size_t remainingLen(util::ContigBlock& contig, size_t startp, bool moveFw);
  void append(std::string& seq, util::ContigBlock& contig, size_t startp, size_t endp, bool moveFw);
  void appendByLen(std::string& seq, util::ContigBlock& contig, size_t startp, size_t len, bool moveFw);
  void cutoff(std::string& seq, size_t len);
  std::string rc(std::string str);
  char rev(const char& c);
  std::vector<nextCompatibleStruct> fetchSuccessors(util::ContigBlock& contig,
                                                 bool moveFw,
                                                 size_t tid,
                                                    size_t tpos);
};

#endif
