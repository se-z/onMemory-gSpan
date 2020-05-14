#ifndef FULLN_T_H
#define FULLN_T_H

#include "projected.h"
#include "tree.hh" // third party library. http://tree.phi-sci.com/

using PROJECT::DFSCode;
using PROJECT::EdgeList;
using PROJECT::get_backward;
using PROJECT::get_forward_pure;
using PROJECT::get_forward_rmpath;
using PROJECT::get_forward_root;
using PROJECT::gTraverse;
using PROJECT::History;
using PROJECT::PDFS;
using PROJECT::Projected;
using PROJECT::Projected_iterator1;
using PROJECT::Projected_iterator2;
using PROJECT::Projected_iterator3;
using PROJECT::Projected_map2;
using PROJECT::Projected_map3;
using PROJECT::RMPath;

using std::cout;
using std::endl;
using std::string;

namespace FULL_TRAVERSE {

class Content {
  public:
    bool nextIsExist = false;
    DFSCode dfscode;
    Projected *projected = NULL;
    Projected_map3 new_fwd_root;
    Projected_map2 new_bck_root;
    Content(){};
    Content(DFSCode &dfscode, Projected *pjtd) {
        this->dfscode = dfscode, this->projected = pjtd;
    };
};

class gFullNodeTraverser : gTraverse {

  private:
    void __traverse(int, const tree<Content>::iterator &);
    const int DATA_SIZE;
    const int MAX_DEPTH;
    const int MIN_SUP;
    vector<Graph> *TRANSACTION;
    void createRoot();
    void createChildren(const tree<Content>::iterator &);

    tree<Content> Lexicographic_Order_Tree;
    tree<Content>::iterator TREE_ROOT; // これをconstにできるかチェックする

  public:
    void traverse();
    double traverse_space_size;
    gFullNodeTraverser(vector<Graph> *TRANS, int min_sup, int max_depth);
    // tree<Content> Lexicographic_Order_Tree;
};

} // namespace FULL_TRAVERSE

#endif