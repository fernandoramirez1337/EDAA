#include <iostream>
#include <vector>
#include <cmath>
#include <queue>

class KDTree {
public:
    struct KDNode {
        KDNode* left = nullptr;
        KDNode* right = nullptr;
        std::vector<int> point;
        
        KDNode(const std::vector<int>& props) : point(props), numDims(props.size()) {}
        
        int get(int depth) const {
            return point[depth % numDims];
        }
        
        int size() const {
            return point.size();
        }
    private:
        int numDims;
    };
    
    struct KDPoint {
        std::vector<int> props;
        
        KDPoint(const std::vector<int>& props) : props(props) {}
        
        int get(int depth) const {
            return props[depth % props.size()];
        }
        
        int size() const {
            return props.size();
        }
    };

    KDTree(int numDims) : numDims(numDims) {}
    
    KDTree(KDNode* root) : root(root), numDims(root->size()) {}
    
    KDTree(const std::vector<std::vector<int>>& points) {
        numDims = points[0].size();
        root = new KDNode(points[0]);
        
        for (size_t i = 1, numPoints = points.size(); i < numPoints; i++) {
            KDNode* n = new KDNode(points[i]);
            add(root, n);
        }
    }

    ~KDTree() {
        deleteTree(root);
    }

    void add(KDNode* point) {
        if (root == nullptr) {
            root = point;
        } else {
            add(root, point);
        }
    }

    KDNode* nearestNeighbor(const KDPoint& target) {
        return nearestNeighbor(root, target, 0);
    }

    void printTree() {
        std::queue<KDNode*> q;
        q.push(root);

        while (!q.empty()) {
            int size = q.size();
            for (int i = 0; i < size; i++) {
                KDNode* n = q.front();
                q.pop();
                if (n != nullptr) {
                    std::cout << "(point: ";
                    for (int prop : n->point) {
                        std::cout << prop << " ";
                    }
                    std::cout << "), ";
                    q.push(n->left);
                    q.push(n->right);
                } else {
                    std::cout << "null, ";
                }
            }
            std::cout << std::endl;
        }
    }

private:
    KDNode* root = nullptr;
    int numDims;

    void add(KDNode* root, KDNode* n, int k = 0) {
        if (n->get(k) < root->get(k)) {
            if (root->left == nullptr) {
                root->left = n;
            } else {
                add(root->left, n, k + 1);
            }
        } else {
            if (root->right == nullptr) {
                root->right = n;
            } else {
                add(root->right, n, k + 1);
            }
        }
    }

    KDNode* nearestNeighbor(KDNode* root, const KDPoint& target, int depth) {
        if (root == nullptr) return nullptr;

        KDNode* nextBranch = nullptr;
        KDNode* otherBranch = nullptr;

        if (target.get(depth) < root->get(depth)) {
            nextBranch = root->left;
            otherBranch = root->right;
        } else {
            nextBranch = root->right;
            otherBranch = root->left;
        }

        KDNode* temp = nearestNeighbor(nextBranch, target, depth + 1);
        KDNode* best = closest(temp, root, target);

        long radiusSquared = distSquared(target, best->point);

        long dist = target.get(depth) - root->get(depth);

        if (radiusSquared >= dist * dist) {
            temp = nearestNeighbor(otherBranch, target, depth + 1);
            best = closest(temp, best, target);
        }

        return best;
    }

    KDNode* closest(KDNode* n0, KDNode* n1, const KDPoint& target) {
        if (n0 == nullptr) return n1;

        if (n1 == nullptr) return n0;

        long d1 = distSquared(n0->point, target);
        long d2 = distSquared(n1->point, target);

        if (d1 < d2)
            return n0;
        else
            return n1;
    }

    static long distSquared(const KDPoint& p0, const KDPoint& p1) {
        long total = 0;
        int numDims = p0.props.size();

        for (int i = 0; i < numDims; i++) {
            int diff = std::abs(p0.get(i) - p1.get(i));
            total += std::pow(diff, 2);
        }
        return total;
    }

    void deleteTree(KDNode* node) {
        if (node == nullptr) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
};

int main() {
    std::vector<std::vector<int>> points = {
        {50, 50, 1},
        {80, 40, 2},
        {10, 60, 3},
        {51, 38, 4},
        {48, 38, 5}
    };

    KDTree tree(2);

    for (const auto& coord : points) {
        KDTree::KDNode* n = new KDTree::KDNode(coord);
        tree.add(n);
    }

    tree.printTree();

    KDTree::KDPoint target({40, 40});
    KDTree::KDNode* nearest = tree.nearestNeighbor(target);
    if (nearest != nullptr) {
        std::cout << "Nearest Neighbor: (";
        for (int prop : nearest->point) {
            std::cout << prop << " ";
        }
        std::cout << ")" << std::endl;
    }

    return 0;
}
