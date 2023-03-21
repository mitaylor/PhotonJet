#ifndef TEXT_H
#define TEXT_H

#include <string>

std::string to_text(float number) {
    auto input_string = std::to_string(number);
    auto decimal = input_string.find('.');

    std::string output_string;

    for (int i = input_string.size() - 1; i >= 0; i--) {
        if (i > decimal && input_string[i] == '0' && output_string.size() == 0) { continue; }
        if (input_string[i] == '.' && output_string.size() == 0) { continue; }

        output_string = input_string[i] + output_string;
    }
    
    return output_string;
}

#endif /* TEXT_H */
