#include <cmath>
#include <iostream>
#include <map>
#include <ostream>
#include <utility>

#define DEBUG_ENABLED 0

#if DEBUG_ENABLED
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

class Utils {
public:
  static void printRanksData(RankData rankData) {
    for (const auto &[key, value] : rankData) {
      std::cout << "[" << key << "]" << " = " << value << std::endl;
    }
  }

  static void printRanksDataFinal(RankData rankData, RankDef ranks) {
    for (const auto &[key, weight] : rankData) {
      for (const auto &criteria : ranks[key]) {
        std::cout << "[" << criteria << "] = " << weight << std::endl;
      }
    }
  }

  static void printUnorderedRanksData(
      std::vector<std::pair<unsigned int, DataPrecision>> ratio) {
    for (const auto &val : ratio) {
      std::cout << "[" << val.first << "]" << " = " << val.second << std::endl;
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
  unsigned int nCards = 0;
  unsigned int nWhiteCards = 0;
  RankData m;
  RankData toRound;
  unsigned int mTotal = 0;
  DataPrecision ratio = 0;
  DataPrecision totalNonNormalizedAll = 0;
  DataPrecision totalNormalizedAll = 0;
  DataPrecision totalNormalizedAllTruncated = 0;
  RankData ratio1;
  RankData ratio2;
  std::vector<std::pair<unsigned int, DataPrecision>> sortedRatio2;
  unsigned int v;

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

  void generateRatios() {
    for (const auto &[key, value] : this->ranks) {
      this->ratio1[key] = Utils::roundToXDecimals(
          (pow(10, -W) - (this->normalizedWeights[key] -
                          this->normalizedWeightsTruncted[key])) /
              this->normalizedWeights[key],
          9);
      this->ratio2[key] = Utils::roundToXDecimals(
          (normalizedWeights[key] - normalizedWeightsTruncted[key]) /
              normalizedWeights[key],
          9);
    }
  }

  void mapM() {
    for (const auto &[key, value] : this->ranks) {
      if (this->ratio1[key] > this->ratio2[key]) {
        // Does not matter what is in here, olny if true
        this->m[key] = 1.0;
        this->mTotal += value.size();
      }
    }
  }

  void sortRatio2() {
    // Copying data from ratio2
    for (const auto &[key, value] : this->ratio2) {
      std::pair<unsigned int, DataPrecision> val;
      val.first = key;
      val.second = value;
      this->sortedRatio2.push_back(val);
    }

    std::sort(this->sortedRatio2.begin(), this->sortedRatio2.end(),
              [](const auto &a, const auto &b) { return a.second > b.second; });
  }

  void mapRanksToRound() {
    unsigned int count = 0;
    for (const auto &value : this->sortedRatio2) {
      if (count >= this->v)
        break;
      if (this->m[value.first])
        continue;
      for (auto &_ : this->ranks[value.first]) {
        count++;
        this->toRound[value.first] = value.second;
      }
    }
  }

public:
  SimosRevised(RankDef ranks, WhiteCardsDef whiteCards) {
    this->ranks = ranks;
    this->whiteCards = whiteCards;
  }
  RankData generateWeights() {
    this->calculateTotalCards();
    this->calculateTotalWhiteCars();
    this->ratio = Utils::roundToXDecimals((Z - 1) / this->nWhiteCards, 6);
    this->calculateNonNormalizedWeights();
    this->calculateTotalNonNormalized();
    this->calculateNormalizedWeights();
    this->generateRatios();
    this->mapM();
    this->v = int(pow(10, W) * (100 - this->totalNormalizedAllTruncated));
    this->sortRatio2();
    this->mapRanksToRound();
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
    if (DEBUG_ENABLED) {
      std::cout << "==============RATIO 1 BEGIN==============" << std::endl;
      Utils::printRanksData(this->ratio1);
      std::cout << "==============RATIO 1 END==============" << std::endl;
    }
    if (DEBUG_ENABLED) {
      std::cout << "==============RATIO 2 BEGIN==============" << std::endl;
      Utils::printRanksData(this->ratio2);
      std::cout << "==============RATIO 2 END==============" << std::endl;
    }
    DEBUG("Total Normalized: %f", this->totalNormalizedAll);
    DEBUG("Total Normalized Truncated: %f", this->totalNormalizedAllTruncated);
    if (DEBUG_ENABLED) {
      std::cout << "==============M BEGIN==============" << std::endl;
      Utils::printRanksData(this->m);
      std::cout << "==============M END==============" << std::endl;
    }
    DEBUG("M total: %d: ", this->mTotal);
    DEBUG("V: %d ", this->v);
    if (DEBUG_ENABLED) {
      std::cout << "==============SORTED RATIO 2 BEGIN=============="
                << std::endl;
      Utils::printUnorderedRanksData(this->sortedRatio2);
      std::cout << "==============SORTED RATIO 2 END=============="
                << std::endl;
    }
    if (DEBUG_ENABLED) {
      std::cout << "==============TO ROUND BEGIN==============" << std::endl;
      Utils::printRanksData(this->toRound);
      std::cout << "==============TO ROUND END==============" << std::endl;
    }

    for (const auto &[key, value] : this->ranks) {
      if (this->toRound[key]) {
        this->normalizedWeightsFinal[key] =
            Utils::roundToXDecimals(normalizedWeights[key], 1);
      } else {
        this->normalizedWeightsFinal[key] =
            Utils::truncateToXDecimals(normalizedWeights[key], 1);
      }
    }

    return this->normalizedWeightsFinal;
  }
};

int main() {
  RankDef ranks = {
      {1, RankArray{"c", "g", "l"}},      {2, RankArray{"d"}},
      {3, RankArray{"b", "f", "i", "j"}}, {4, RankArray{"e"}},
      {5, RankArray{"a", "h"}},           {6, RankArray{"k"}},
  };

  WhiteCardsDef whiteCards = {{1, 0}, {2, 1}, {3, 0}, {4, 0}, {5, 0}};

  SimosRevised simos = SimosRevised(ranks, whiteCards);

  RankData finalWeights = simos.generateWeights();

  Utils::printRanksDataFinal(finalWeights, ranks);

  return 0;
}
