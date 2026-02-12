#include "LevelLoader.h"
#include "cocos2d.h"

USING_NS_CC;

GameModel* LevelLoader::load(const std::string& filename) {
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    auto content = FileUtils::getInstance()->getStringFromFile(fullPath);
    if (content.empty()) return nullptr;

    // 移除UTF-8 BOM（如果存在）
    if (content.length() >= 3 && (unsigned char)content[0] == 0xEF &&
        (unsigned char)content[1] == 0xBB && (unsigned char)content[2] == 0xBF)
        content = content.substr(3);

    // 辅助lambda：从JSON对象字符串中提取整数
    auto extractInt = [](const std::string& s, const std::string& key) {
        auto p = s.find("\"" + key + "\":");
        if (p == std::string::npos) return -1;
        p += key.length() + 3;
        while (p < s.length() && (s[p] == ' ' || s[p] == '\t')) p++;
        int v = 0;
        while (p < s.length() && isdigit(s[p])) v = v * 10 + (s[p++] - '0');
        return v;
        };

    GameModel* model = new GameModel();

    // 解析主牌区（Playfield）
    auto pf = content.find("\"Playfield\"");
    if (pf != std::string::npos) {
        auto start = content.find('[', pf), end = content.find(']', start);
        if (start != std::string::npos && end != std::string::npos) {
            std::string arr = content.substr(start + 1, end - start - 1);
            size_t pos = 0;
            while ((pos = arr.find('{', pos)) != std::string::npos) {
                auto ce = arr.find('}', pos);
                if (ce == std::string::npos) break;
                std::string obj = arr.substr(pos + 1, ce - pos - 1);
                int face = extractInt(obj, "CardFace") + 1; // JSON中0→A,1→2,...,12→K
                int suit = extractInt(obj, "CardSuit");
                int x = extractInt(obj, "x"), y = extractInt(obj, "y");
                CardModel* card = new CardModel((CardFace)face, (CardSuit)suit, true);
                card->setPosition(Vec2(x, y));
                model->addPlayFieldCard(card);
                pos = ce + 1;
            }
        }
    }

    // 解析堆牌区（Stack）
    auto st = content.find("\"Stack\"");
    if (st != std::string::npos) {
        auto start = content.find('[', st), end = content.find(']', start);
        if (start != std::string::npos && end != std::string::npos) {
            std::string arr = content.substr(start + 1, end - start - 1);
            size_t pos = 0;
            while ((pos = arr.find('{', pos)) != std::string::npos) {
                auto ce = arr.find('}', pos);
                if (ce == std::string::npos) break;
                std::string obj = arr.substr(pos + 1, ce - pos - 1);
                int face = extractInt(obj, "CardFace") + 1;
                int suit = extractInt(obj, "CardSuit");
                CardModel* card = new CardModel((CardFace)face, (CardSuit)suit, true);
                model->addStackCard(card);
                pos = ce + 1;
            }
        }
    }

    return model;
}