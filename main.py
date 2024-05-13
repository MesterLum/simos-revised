Z = 6.5
W = 1


def round_to_x_decimals(n, n_decimals):
    return round(n * 10**n_decimals) / 10**n_decimals


def truncate_to_x_decimals(n, n_decimals):
    return int(n * 10**n_decimals) / 10 * n_decimals


def print_results(ranks, weights):
    for k, values in ranks.items():
        for v in values:
            print(f"RANK {k} Criteria {v} Weight {weights[k]}")


if __name__ == "__main__":
    ranks = {
        1: ["c", "g", "l"],
        2: ["d"],
        3: ["b", "f", "i", "j"],
        4: ["e"],
        5: ["a", "h"],
        6: ["k"],
    }
    white_cards = {
        1: 0,
        2: 1,
        3: 0,
        4: 0,
        5: 0,
    }
    total_weights = {}
    non_normalized_weights = {}
    normalized_weights = {}
    normalized_weights_truncated = {}
    normalized_weights_final = {}
    ratio_1 = {}
    ratio_2 = {}
    total_non_normalized_all = 0
    total_normalized_all = 0
    total_normalized_truncated_all = 0
    M = {}

    n_cards = 0
    n_white_cards = 0
    m = 0
    # Accum total number of cards
    for _, v in ranks.items():
        n_cards += len(v)

    # Accum total number of white cards
    # Adding e'[r] + 1
    for k, v in white_cards.items():
        white_cards[k] += 1
        n_white_cards += white_cards[k]

    ratio = round_to_x_decimals((Z - 1) / n_white_cards, 6)

    # Calculing normalized weights
    skipped = False
    for k, v in ranks.items():
        if not skipped:
            non_normalized_weights[k] = 1
            skipped = True
        else:
            total_white_cards = 0
            for i in range(k - 1):
                total_white_cards += white_cards[i + 1]
                non_normalized_weights[k] = round_to_x_decimals(
                    1 + ratio * total_white_cards, 2
                )

        # getting total
        total = round_to_x_decimals(len(v) * non_normalized_weights[k], 2)
        total_non_normalized_all += total
        total_weights[k] = total

    # Calculing normalized weights
    for k, v in ranks.items():
        total = 100 / total_non_normalized_all * non_normalized_weights[k]
        total_truncated = truncate_to_x_decimals(total, W)
        total_normalized_all += total * len(v)
        total_normalized_truncated_all += total_truncated * len(v)
        normalized_weights[k] = total
        normalized_weights_truncated[k] = total_truncated
        ratio_1[k] = round_to_x_decimals(
            (10**-W - (normalized_weights[k] - normalized_weights_truncated[k]))
            / normalized_weights[k],
            9,
        )
        ratio_2[k] = round_to_x_decimals(
            (normalized_weights[k] - normalized_weights_truncated[k])
            / normalized_weights[k],
            9,
        )

        if ratio_1[k] > ratio_2[k]:
            M[k] = 1
            m += len(v)

    # l_1 = dict(sorted(ratio_1.items(), key=lambda x: x[1]))
    l_2 = dict(sorted(ratio_2.items(), key=lambda x: x[1], reverse=True))

    #
    e = 100 - total_normalized_truncated_all
    v = int(10**W * e)

    count = 0
    to_round = {}
    for k, _ in l_2.items():
        if M.get(k) is not None:
            continue

        if count >= v:
            break

        for idx in enumerate(ranks[k]):
            count = count + 1
            to_round[k] = 1

    for k, _ in ranks.items():
        if to_round.get(k):
            normalized_weights_final[k] = round_to_x_decimals(normalized_weights[k], 1)
        else:
            normalized_weights_final[k] = truncate_to_x_decimals(
                normalized_weights[k], 1
            )

    # print("E", e, v, m)
    # print("Total cards:", n_cards)
    # print("white_cards:", n_white_cards)
    # print("ratio:", ratio)
    # print("non normalized weightd:", non_normalized_weights)
    # print("total weights:", total_weights)
    # print("total non normalized accumed:", total_non_normalized_all)
    # print("normalized weights:", normalized_weights)
    # print("total normalized accum:", total_normalized_all)
    # print("total normalized truncated:", total_normalized_truncated_all)
    # print("normalized weights truncated:", normalized_weights_truncated)
    # print("ratio 1 list:", ratio_1)
    # print("ratio 2 list:", ratio_2)
    # print("M GROUP", M)
    # # print("L1: ", l_1)
    # print("L2: ", l_2)
    # print("to round", to_round)
    # print("normalized weights final", normalized_weights_final)
    print_results(ranks, normalized_weights_final)
