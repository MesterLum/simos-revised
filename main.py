if __name__ == "__main__":
    ranks = {
        1: ["c", "g", "l"],
        2: ["d"],
        3: "WHITE_CARD",
        4: ["b", "f", "i", "j"],
        5: ["e"],
        6: ["a", "h"],
        7: ["k"],
    }
    non_normalized_weights = {}
    normalized_weights = {}
    total_weights = {}

    n_cards = 0
    positions = 0
    for k, v in ranks.items():
        if v != "WHITE_CARD":
            sum = 0
            for idx in range(len(v)):
                sum = sum + idx + 1 + n_cards
                positions += idx + 1 + n_cards
            non_normalized_weights[k] = sum / len(v)
            n_cards = n_cards + len(v)

        else:
            n_cards += 1

    for k, v in non_normalized_weights.items():
        normalized_weights[k] = round(v / positions * 100)
        total_weights[k] = len(ranks[k]) * normalized_weights[k]

    print("# Cards", n_cards)
    print("Sum cards", positions)
    print("Non normalized weights", non_normalized_weights)
    print("Normalized weights", normalized_weights)
    print("Total", total_weights)
