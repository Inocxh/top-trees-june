#include "two_edge_connected.h"

void TwoEdgeConnectivity::cover(int u, int v, int level) {
    TwoEdgeCluster *root = this->top_tree->expose(u, v);
    root->cover(level);
    this->top_tree->deexpose(u, v);
}

void TwoEdgeConnectivity::uncover(int u, int v, int level) {
     TwoEdgeCluster *root = this->top_tree->expose(u, v);
    root->uncover(level);
    this->top_tree->deexpose(u, v);
}

NewEdge* TwoEdgeConnectivity::insert(int u, int v) {
    //Try to link u,v in tree
    NewEdge edge_data = NewEdge::tree_edge(u, v, -1, nullptr);
    TwoEdgeCluster* result = this->top_tree->link_leaf(u, v, edge_data); //TODO level = lmax?
    
    if (result) {
        //If successfull, try to assign vertex endpoints to new leaf
        if (!vertex_labels[u]->leaf_node) {
            result->assign_vertex(u, vertex_labels[u]);
            vertex_labels[u]->leaf_node = result;
        }
        if (!vertex_labels[v]->leaf_node) {
            result->assign_vertex(v, vertex_labels[v]);
            vertex_labels[v]->leaf_node = result;
        }
        result->full_splay();
        result->recompute_root_path();
        return NewEdge::new_tree_edge(u, v, -1, result);
    }
    NewEdge* edge = NewEdge::new_non_tree_edge(u, v, 0);
    this->add_label(u, edge);
    this->add_label(v, edge);
    this->cover(u, v, 0);
    return edge;
}

NewEdge* TwoEdgeConnectivity::insert(int u, int v, int level) {
    TwoEdgeCluster* result = this->top_tree->link_leaf(u, v, NewEdge::tree_edge(u, v, -1, nullptr)); //TODO level = lmax?
    if (result) {
        return NewEdge::new_tree_edge(u, v, -1, result);
    }
    NewEdge* edge = NewEdge::new_non_tree_edge(u, v, level);
    this->add_label(u, edge);
    this->add_label(v, edge);
    this->cover(u, v, level);
    return edge;
}

void TwoEdgeConnectivity::add_label(int vertex, NewEdge* edge) {
    VertexLabel* vertex_label = this->vertex_labels[vertex];
    int index = vertex_label->labels[edge->level].size();
    if (edge->endpoints[0] == vertex) {
        edge->extra_data.index[0] = index;
    } else {
        edge->extra_data.index[1] = index;
    }
    vertex_label->labels[edge->level].push_back(edge);
    
    assert(vertex_label->leaf_node);
    vertex_label->leaf_node->full_splay(); //depth <= 5
    vertex_label->leaf_node->recompute_root_path(); //takes O(depth) = O(1) time
}

void TwoEdgeConnectivity::remove_labels(NewEdge* edge) {
    int level = edge->level;
    for (int i = 0; i < 2; i++) {
        int ep = edge->endpoints[i];
        int ep_idx = edge->extra_data.index[i];

        VertexLabel* ep_label = this->vertex_labels[ep];
        
        NewEdge* last_label = ep_label->labels[level].back();
        assert(last_label->edge_type == NonTreeEdge1);
        int ep_is_right_new = last_label->endpoints[1] == ep;

        ep_label->labels[level][ep_idx] = last_label;
        ep_label->labels[level][ep_idx]->extra_data.index[ep_is_right_new] = ep_idx;
        ep_label->labels[level].pop_back();

        if (ep_label->leaf_node) {
            ep_label->leaf_node->full_splay();
            ep_label->leaf_node->recompute_root_path();
        }
    }
}   

void TwoEdgeConnectivity::reassign_vertices(TwoEdgeCluster* leaf_node) {
    for (int i = 0; i < 2; i++) {
        if (leaf_node->vertex[i]) {
            //find new edge
            int id = leaf_node->get_endpoint_id(i);
            TwoEdgeCluster* replacement = this->top_tree->get_adjacent_leaf_node(id);
            if (replacement == leaf_node) {
                replacement = this->top_tree->get_adjacent_leaf_node(id, 1);
                if (!replacement) {
                    leaf_node->vertex[i]->leaf_node = nullptr;
                    continue;
                }
            } 
            replacement->assign_vertex(id, leaf_node->vertex[i]);
            leaf_node->vertex[i]->leaf_node = replacement;
            leaf_node->vertex[i] = nullptr;

            replacement->full_splay();
            replacement->recompute_root_path();
        }
    }

}


void TwoEdgeConnectivity::remove(NewEdge* edge) {
    int u = edge->endpoints[0];
    int v = edge->endpoints[1];

    if (edge->edge_type == TreeEdge1) {
        int cover_level = this->cover_level(u, v);
        if (cover_level == -1) {
            reassign_vertices(edge->extra_data.leaf_node);
            top_tree->cut_leaf(edge->extra_data.leaf_node);
            return;
        }
        edge = this->swap(edge);
    } 
    assert(edge->edge_type == NonTreeEdge1);
    this->remove_labels(edge);
    this->uncover(u, v, edge->level);
    for (int i = edge->level; i >= 0; i--) {
        this->recover(v, u, i);
    }

}

int TwoEdgeConnectivity::cover_level(int u, int v) {
    TwoEdgeCluster* root = this->top_tree->expose(u, v);
    int cover_level = root->get_cover_level();
    this->top_tree->deexpose(u, v);
    return cover_level;    
}

NewEdge* TwoEdgeConnectivity::swap(NewEdge* tree_edge) {
    int u = tree_edge->endpoints[0];
    int v = tree_edge->endpoints[1];
    int cover_level = this->cover_level(u, v);
    reassign_vertices(tree_edge->extra_data.leaf_node);
    auto root = this->top_tree->cut_leaf(tree_edge->extra_data.leaf_node);


    NewEdge* non_tree_edge = find_replacement(u, v, cover_level);
    int x = non_tree_edge->endpoints[0];
    int y = non_tree_edge->endpoints[1];
    this->remove_labels(non_tree_edge);
    
    NewEdge e = NewEdge::tree_edge(x,u,-1,nullptr);
    TwoEdgeCluster* new_leaf = this->top_tree->link_leaf(x, y, e);
    if (new_leaf) {
        //If successfull, try to assign vertex endpoints to new leaf
        if (!vertex_labels[x]->leaf_node) {
            new_leaf->assign_vertex(x, vertex_labels[x]);
            vertex_labels[x]->leaf_node = new_leaf;
        }
        if (!vertex_labels[y]->leaf_node) {
            new_leaf->assign_vertex(y, vertex_labels[y]);
            vertex_labels[y]->leaf_node = new_leaf;
        }
        new_leaf->full_splay();
        new_leaf->recompute_root_path();
    }

    // TODO comment
    NewEdge::swap(non_tree_edge,NewEdge::new_tree_edge(x,y,-1,new_leaf));

    
    NewEdge* edge = NewEdge::new_non_tree_edge(u, v, cover_level);
    this->add_label(u, edge);
    this->add_label(v, edge);
    this->cover(u, v, cover_level);
    return edge;
}

int TwoEdgeConnectivity::find_size(int u, int v, int cover_level) {
    TwoEdgeCluster* root = this->top_tree->expose(u);
    if (u != v) {
        root = this->top_tree->expose(v);
    }
    int size;
    if (root) {
        size = root->get_size(cover_level);
    } else {
        size = 1;
    }
    this->top_tree->deexpose(u);
    if (u != v) {
        this->top_tree->deexpose(v);
    }
    return size;
}

NewEdge* TwoEdgeConnectivity::find_replacement(int u, int v, int cover_level) {
    int size_u = this->find_size(u, u, cover_level);
    int size_v = this->find_size(v, v, cover_level);
    if (size_u <= size_v) {
        return this->recover_phase(u, u, cover_level, size_u);
    } else {
        return this->recover_phase(v, v, cover_level, size_v);
    }
}

NewEdge* TwoEdgeConnectivity::find_first_label(int u, int v, int cover_level) {
    TwoEdgeCluster* root = this->top_tree->expose(u);
    NewEdge* res;
    VertexLabel* label;

    if (u != v) {
        root = this->top_tree->expose(v);
    }
    if (!root) { // if no root, no tree edges. Look for label in vertex only.
        assert(u == v);

        label = vertex_labels[u];
        if (label->labels[cover_level].size() > 0) {
            res = label->labels[cover_level].back();
        } else {
            res = nullptr;
        }
        goto out;
    }
    label = root->find_first_label(u, v, cover_level); 
    if (!label) {
        res = nullptr;
        goto out;
    }
    res = label->labels[cover_level].back();
    
    out:
    this->top_tree->deexpose(u);
    if (u != v) {
        this->top_tree->deexpose(v);
    }
    return res;
}

void TwoEdgeConnectivity::recover(int u, int v, int cover_level) {
    int size = this->find_size(u,v,cover_level) / 2;
    this->recover_phase(u, v, cover_level, size);
    this->recover_phase(v, u, cover_level, size);
}

NewEdge* TwoEdgeConnectivity::recover_phase(int u, int v, int cover_level, int size) {
    NewEdge* label = this->find_first_label(u, v, cover_level);
    while (label) {
        int q = label->endpoints[0];
        int r = label->endpoints[1];
        if (!this->top_tree->connected(q, r)) {
            return label;
        }
        if (this->find_size(q, r, cover_level + 1) <= size) {
            this->remove_labels(label);
            label->level = cover_level + 1;
            this->add_label(q, label);
            this->add_label(r, label);
            this->cover(q, r, cover_level + 1);
        } else {
            this->cover(q, r, cover_level); 
            return nullptr;
        }
        label = this->find_first_label(u, v, cover_level);
    }
    return nullptr;
}

bool TwoEdgeConnectivity::two_edge_connected(int u, int v) {
    return (this->top_tree->connected(u,v) && (this->cover_level(u,v) >= 0));
}

NewEdge* TwoEdgeConnectivity::find_bridge(int u, int v) {
    TwoEdgeCluster* root = this->top_tree->expose(u,v);
    NewEdge* bridge;
    if (root->cover_level == -1) {
        bridge = root->min_path_edge;
    } else {
        bridge = nullptr;
    }
    this->top_tree->deexpose(u,v);
    return bridge;
}