#include "fullNodeTraverse.h"

namespace FULL_TRAVERSE {

gFullNodeTraverser::gFullNodeTraverser(vector<Graph> *TRANS, int min_sup,
                                       int max_depth)
    : TRANSACTION(TRANS), MAX_DEPTH(max_depth), MIN_SUP(min_sup),
      DATA_SIZE((*TRANS).size()) {
    createRoot();
};

void gFullNodeTraverser::createRoot() {

    DFS_CODE.clear();
    EdgeList edges;

    TREE_ROOT = Lexicographic_Order_Tree.insert(
        Lexicographic_Order_Tree.begin(), Content());
    Projected_map3 &root = TREE_ROOT->new_fwd_root;
    for(int graph_id = 0; graph_id < DATA_SIZE; ++graph_id) {
        Graph &g = (*TRANSACTION)[graph_id];
        for(unsigned int from = 0; from < g.size(); ++from) {
            if(get_forward_root(g, g[from], edges)) {
                for(auto &edg : edges) {
                    root[g[from].label][edg->elabel][g[edg->to].label].push(
                        graph_id, edg, 0);
                }
            }
        }
    }

    for(Projected_iterator3 fromlabel = root.begin(); fromlabel != root.end();
        ++fromlabel) {
        for(Projected_iterator2 elabel = fromlabel->second.begin();
            elabel != fromlabel->second.end(); ++elabel) {
            for(Projected_iterator1 tolabel = elabel->second.begin();
                tolabel != elabel->second.end(); ++tolabel) {

                DFS_CODE.push(0, 1, fromlabel->first, elabel->first,
                              tolabel->first);
                int depth = 0;
                if(is_min()) {
                    Projected *projected = &(tolabel->second);
                    Lexicographic_Order_Tree.append_child(
                        TREE_ROOT, Content(DFS_CODE, projected));
                }
                DFS_CODE.pop();
            }
        }
    }
}
void gFullNodeTraverser::createChildren(const tree<Content>::iterator &node) {

    if(!node->nextIsExist) {

        DFS_CODE = node->dfscode;

        const RMPath &rmpath = DFS_CODE.buildRMPath();
        int minlabel = DFS_CODE[0].fromlabel;
        int maxtoc = DFS_CODE[rmpath[0]].to;
        EdgeList edges;
        for(unsigned int itre = 0; itre < (*(node->projected)).size(); ++itre) {
            PDFS *cur = &(*node->projected)[itre];
            unsigned int id = (*node->projected)[itre].id;

            History history((*TRANSACTION)[id], cur);
            for(int i = (int)rmpath.size() - 1; i >= 1; --i) {
                Edge *e = get_backward((*TRANSACTION)[id], history[rmpath[i]],
                                       history[rmpath[0]], history);
                if(e) {
                    node->new_bck_root[DFS_CODE[rmpath[i]].from][e->elabel]
                        .push(id, e, cur);
                }
            }

            if(get_forward_pure((*TRANSACTION)[id], history[rmpath[0]],
                                minlabel, history, edges)) {
                for(auto &e : edges) {
                    node->new_fwd_root[maxtoc][e->elabel]
                                      [(*TRANSACTION)[id][e->to].label]
                                          .push(id, e, cur);
                }
            }

            for(int i = 0; i < (int)rmpath.size(); ++i) {
                if(get_forward_rmpath((*TRANSACTION)[id], history[rmpath[i]],
                                      minlabel, history, edges)) {
                    for(auto &e : edges) {
                        node->new_fwd_root[DFS_CODE[rmpath[i]].from][e->elabel]
                                          [(*TRANSACTION)[id][e->to].label]
                                              .push(id, e, cur);
                    }
                }
            }
        }

        node->nextIsExist = true;
        for(auto &[to, mp] : node->new_bck_root) {
            for(auto &[elabel, prj] : mp) {
                DFS_CODE.push(maxtoc, to, -1, elabel, -1);
                if(is_min()) {
                    Lexicographic_Order_Tree.append_child(
                        node, Content(DFS_CODE, &prj));
                }

                DFS_CODE.pop();
            }
        }

        for(auto &[from, mp] : node->new_fwd_root) {
            for(auto &[elabel, prj] : mp) {
                for(auto &[key, val] : prj) {
                    DFS_CODE.push(from, maxtoc + 1, -1, elabel, key);
                    if(is_min()) {
                        Lexicographic_Order_Tree.append_child(
                            node, Content(DFS_CODE, &val));
                    }

                    DFS_CODE.pop();
                }
            }
        }
    }
}

void gFullNodeTraverser::traverse() {

    //  初期化
    DFS_CODE.clear();

    int test_itr = 0;
    for(tree<Content>::sibling_iterator it =
            Lexicographic_Order_Tree.begin(TREE_ROOT);
        it != Lexicographic_Order_Tree.end(TREE_ROOT); ++it) {
        test_itr++;
    }

    for(tree<Content>::sibling_iterator it =
            Lexicographic_Order_Tree.begin(TREE_ROOT);
        it != Lexicographic_Order_Tree.end(TREE_ROOT); ++it) {
        int depth = 0;
        __traverse(depth, it);
    }

    return;
}

void gFullNodeTraverser::__traverse(int depth,
                                    const tree<Content>::iterator &node) {

    depth++;
    if(depth > MAX_DEPTH) {
        return;
    }

    if((*(node->projected)).size() > MIN_SUP) {
        cout << "---------------------------" << endl;
        cout << "[INFO] frequent pattern : size = " << depth << endl;
        DFSCode Normarize_DFS;
        DFS_CODE.normalizeRM(Normarize_DFS);
        cout << Normarize_DFS << endl;
    }

    createChildren(node);

    for(tree<Content>::sibling_iterator it =
            Lexicographic_Order_Tree.begin(node);
        it != Lexicographic_Order_Tree.end(node); ++it) {
        __traverse(depth, it);
    }

    return;
}

} // namespace FULL_TRAVERSE