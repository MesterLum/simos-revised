#include <cmath>
#include <iostream>
#include <map>
#include <ostream>

#define DEBUG_ENABLED 1

#ifdef DEBUG_ENABLED
#define DEBUG(string, ...)                                                     \
  (printf("DEBUG [%s %s]: ", __DATE__, __TIME__), printf(string, __VA_ARGS__), \
   printf("\n"))
#else
#define DEBUG(string, ...)
#endif

#define Z 6.5
#define W 1
typedef double DataPrecision;
typedef std::vector<std::string> RankArray;
typedef std::map<unsigned int, RankArray> RankDef;
typedef std::map<unsigned int, DataPrecision> RankData;
typedef std::map<unsigned int, unsigned int> WhiteCardsDef;
typedef std::map<std::string, float> GeneratedWeights;

class Utils {
public:
  static void printFinalWeights(GeneratedWeights &weights) {
    for (const auto &[key, value] : weights) {
      std::cout << "[" << key << "]" << " = " << value << std::endl;
    }
  }
  static void printRanksData(RankData rankData) {
    for (const auto &[key, value] : rankData) {
      std::cout << "[" << key << "]" << " = " << value << std::endl;
    }
  }
  static DataPrecision roundToXDecimals(DataPrecision value,
                                        unsigned int decimals) {
    return round(value * pow(10, decimals)) / pow(10, decimals);
  }
  static DataPrecision truncateToXDecimals(DataPrecision value,
                                           unsigned int decimals) {
    return int(value * pow(10, decimals)) / pow(10, decimals);
  }
};

class SimosRevised {
private:
  RankDef ranks;
  WhiteCardsDef whiteCards;
  RankData nonNormalizedWeights;
  RankData normalizedWeights;
  RankData normalizedWeightsTruncted;
  RankData normalizedWeightsFinal;
  RankData totalNonNormalized;
  unsigned int nCards;
  unsigned int nWhiteCards;
  unsigned int m;
  DataPrecision ratio;
  DataPrecision totalNonNormalizedAll;
  DataPrecision totalNormalizedAll;
  DataPrecision totalNormalizedAllTruncated;

  // method definition
  void calculateTotalCards() {
    for (const auto &[_, ranks] : this->ranks) {
      this->nCards += ranks.size();
    }
  }

  void calculateTotalWhiteCars() {
    for (const auto &[key, _] : this->whiteCards) {
      this->whiteCards[key] += 1;
      this->nWhiteCards += this->whiteCards[key];
    }
  }

  void calculateNonNormalizedWeights() {
    bool firstSkipped = false;

    for (const auto &[key, value] : this->ranks) {
      if (!firstSkipped) {
        this->nonNormalizedWeights[key] = 1.00f;
        firstSkipped = true;
        continue;
      }

      unsigned int totalWhiteCards = 0;

      for (int i = 1; i <= key - 1; i++) {
        totalWhiteCards += this->whiteCards[i];
        this->nonNormalizedWeights[key] =
            Utils::roundToXDecimals(1 + this->ratio * totalWhiteCards, 2);
      }
    }
  }

  void calculateTotalNonNormalized() {
    for (const auto &[key, value] : this->ranks) {
      DataPrecision total = Utils::roundToXDecimals(
          value.size() * this->nonNormalizedWeights[key], 2);
      this->totalNonNormalizedAll += total;
      this->totalNonNormalized[key] = total;
    }
  }

  void calculateNormalizedWeights() {
    for (const auto &[key, value] : this->ranks) {
      DataPrecision total =
          100 / this->totalNonNormalizedAll * this->nonNormalizedWeights[key];
      DataPrecision totalTruncated = Utils::truncateToXDecimals(total, W);
      this->normalizedWeights[key] = total;
      this->normalizedWeightsTruncted[key] = totalTruncated;
      this->totalNormalizedAll += total * value.size();
      this->totalNormalizedAllTruncated += totalTruncated * value.size();
    }
  }

public:
  SimosRevised(RankDef ranks, WhiteCardsDef whiteCards) {
    this->ranks = ranks;
    this->whiteCards = whiteCards;
  }
  GeneratedWeights generateWeights() {
    this->calculateTotalCards();
    this->calculateTotalWhiteCars();
    this->ratio = Utils::roundToXDecimals((Z - 1) / this->nWhiteCards, 6);
    this->calculateNonNormalizedWeights();
    this->calculateTotalNonNormalized();
    this->calculateNormalizedWeights();
    DEBUG("Total cards: %d", this->nCards);
    DEBUG("Total white cards: %d", this->nWhiteCards);
    DEBUG("Ratio: %f", this->ratio);
    if (DEBUG_ENABLED) {
      std::cout << "==============NON NORMALIZED WEIGHTS BEGIN=============="
                << std::endl;
      Utils::printRanksData(this->nonNormalizedWeights);
      std::cout << "==============NON NORMALIZED WEIGHTS END=============="
                << std::endl;
    }
    DEBUG("Total Non Normalized Weights: %f", this->totalNonNormalizedAll);
    if (DEBUG_ENABLED) {
      std::cout << "==============NORMALIZED WEIGHTS BEGIN=============="
                << std::endl;
      Utils::printRanksData(this->normalizedWeights);
      std::cout << "==============NORMALIZED WEIGHTS END=============="
                << std::endl;
    }
    if (DEBUG_ENABLED) {
      std::cout
          << "==============NORMALIZED WEIGHTS TRUNCATED BEGIN=============="
          << std::endl;
      Utils::printRanksData(this->normalizedWeightsTruncted);
      std::cout
          << "==============NORMALIZED WEIGHTS TRUNCATED END=============="
          << std::endl;
    }
    DEBUG("Total Normalized: %f", this->totalNormalizedAll);
    DEBUG("Total Normalized Truncated: %f", this->totalNormalizedAllTruncated);

    return GeneratedWeights{{"x", 1.50}};
  }
};

int main() {
  RankDef ranks = {
      {1, RankArray{"c", "g", "l"}},      {2, RankArray{"d"}},
      {3, RankArray{"b", "f", "i", "j"}}, {4, RankArray{"e"}},
      {5, RankArray{"a", "h"}},           {6, RankArray{"k"}},
  };

  WhiteCardsDef whiteCards = {
      {1, 0}, {2, 1}, {3, 0}, {4, 0}, {5, 0},
  };

  SimosRevised simos = SimosRevised(ranks, whiteCards);

  GeneratedWeights finalWeights = simos.generateWeights();

  Utils::printFinalWeights(finalWeights);
  return 0;
}
