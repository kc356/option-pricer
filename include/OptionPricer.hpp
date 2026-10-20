//
// Created by Kin on 10/20/2026.
//

#ifndef OPTION_PRICER_OPTIONPRICER_HPP
#define OPTION_PRICER_OPTIONPRICER_HPP
#include <memory>

#include "Option.hpp"

struct MarketData {
    double spotPrice;
    double riskFreeRate;
};

class OptionPricer {
    public:
    virtual ~OptionPricer() = default;

    virtual double price(Option& option, MarketData& marketData) = 0;
};

struct Node {
    double spotPrice {};
    Node* upNode { nullptr };
    Node* downNode { nullptr };
};

class BinomialOptionPricer final : public OptionPricer {
    public:

    Node* spotPriceTreeRoot_ { nullptr };

    BinomialOptionPricer(const double up, const double down, const int steps)
        : up_(up), down_(down), steps_(steps) {};

    double price(Option& option, MarketData& marketData) override {
        spotPriceTreeRoot_ = new Node {marketData.spotPrice};
        return backwardInduction(
            buildBinaryTree(spotPriceTreeRoot_,spotPriceTreeRoot_, steps_),
            option,
            marketData.riskFreeRate);
    };

    // static void printBinaryTree(const Node* root) {
    //     if (root == nullptr) {
    //         return;
    //     }
    //     std::cout << root->spotPrice << std::endl;
    //     printBinaryTree(root->upNode);
    //     printBinaryTree(root->downNode);
    // }

private:
    double up_;
    double down_;
    int steps_;


    Node* buildBinaryTree(Node* root, Node* currentNode, const int currentStep) {
        if (currentStep == 0) {
            return root;
        }

        currentNode->upNode = new Node {currentNode->spotPrice * (1 + up_)};
        currentNode->downNode = new Node {currentNode->spotPrice * (1 + down_)};
        buildBinaryTree(root, currentNode->upNode, currentStep - 1);
        buildBinaryTree(root, currentNode->downNode, currentStep - 1);
        return root;
    }

    double backwardInduction(const Node* node, Option& option, const double riskFreeRate) {
        if (node->upNode == nullptr && node->downNode == nullptr) {
            return option.payoff(node->spotPrice);
        }

        const double riskNeutralProbability = (riskFreeRate - down_) / (up_ - down_);

        return 1.0 / (1 + riskFreeRate) * (
            riskNeutralProbability * backwardInduction(node->upNode, option, riskFreeRate) +
            (1 - riskNeutralProbability) * backwardInduction(node->downNode, option, riskFreeRate)
        );
    }

};

#endif //OPTION_PRICER_OPTIONPRICER_HPP