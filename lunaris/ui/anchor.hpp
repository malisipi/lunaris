#pragma once
#include <queue>
#include <map>
#include <string>

namespace lunaris::ui::anchor {
    // "x|20%x"
    // "50%x + 20"
    // "50%x + 20 * 4"
    // "x|50%x + 20 * 4 + 20%y"
    // "y|50%x + 20 * (4 + 20%y)"

    inline bool is_math_op(char op){
        return op == '*' || op == '/' || op == '-' || op == '+';
    };

    int apply_op(int n1, int n2, char op){
        if(op == '+') return n1+n2;
        if(op == '-') return n1-n2;
        if(op == '*') return n1*n2;
        if(op == '/'){
            if(n2 == 0) {
                printf("Divide by 0 (%d/0)\n\\->Will be suppresed by returning 1\n", n1);
                return 1;
            };
            return n1/n2;
        };
        // Should never reach here, (until defined new op).
        // Invalid operations never included the op_stack
        printf("Invalid operation (%c)\n\\->Should never reach here\n", op);
        exit(1);
    };

    int calc(int win_width, int win_height, const char* anchor){
        std::queue<int> numbers;
        std::queue<char> ops;
        const int anchor_len = strlen(anchor);
        char will_mirrored_axis = '\0';
        char* sub_reason = (char*)"<Invalid reason>";

        for(int i=0; i<anchor_len; i++){
            const char op = anchor[i];
            if(i == 0 && (op == 'x' || op == 'y')){
                if(anchor_len > 1 && anchor[1] == '|') {
                    will_mirrored_axis = op;
                    i++;
                } else {
                    sub_reason = (char*)"Invalid mirror axis";
                    goto _invalid_format;
                };
            } else if(isdigit(op)){
                int number = op - '0'; // Char codes will be used, for (2); 50 (2) - 48 (0);
                while(i<anchor_len){
                    const char next_op = anchor[i+1];
                    if(isdigit(next_op)){
                        number = number * 10 + (next_op - '0');
                        i++;
                        continue;
                    } else if(next_op == '%') {
                        i++;
                        char axis = anchor[++i];
                        if(axis == 'w'){
                            number = win_width*number/100;
                        } else if(axis == 'h'){
                            number = win_height*number/100;
                        } else {
                            sub_reason = (char*)"Unknown mirror axis";
                            goto _invalid_format;
                        };
                    }
                    break;
                };
                numbers.push(number);
            } else if(op == '('){
                int j = i;
                int parenthesis_count = 0;
                while(true){
                    if(j>=anchor_len){
                        sub_reason = (char*)"Missing char: `)` aka `parenthesis`";
                        goto _invalid_format;
                    };
                    j++;
                    if(anchor[j]=='('){
                        parenthesis_count++;
                    };
                    if(anchor[j]==')'){
                        if(parenthesis_count == 0){
                            break;
                        }
                        parenthesis_count--;
                    };
                };
                char sub_anchor[j-i];
                strncpy(sub_anchor, anchor+(i+1), j-i-1);
                sub_anchor[j-i-1] = '\0';
                numbers.push(calc(win_width, win_height, sub_anchor));
                i=j; // Skip to end of parentheses
            } else if(is_math_op(op)){
                ops.push(op);
            } else if(op == ' ') {
                continue; // Skip is whitespace
            } else {
                sub_reason = (char*)"Unknown operator";
                goto _invalid_format;
            };
        };


        int n1, n2, n3;
        bool n1_v, n2_v, n3_v;
        char op1, op2;
        n1 = n2 = n3 = 0;
        n1_v = n2_v = n3_v = false;
        op1 = op2 = '\0';

        while(true){
            int ops_len = ops.size();
            int n_len = numbers.size();
            if(!n1_v && n_len>0) { n1 = numbers.front(); numbers.pop(); n1_v = true; n_len--; };
            if(!n2_v && n_len>0) { n2 = numbers.front(); numbers.pop(); n2_v = true; n_len--; };
            if(!n3_v && n_len>0) { n3 = numbers.front(); numbers.pop(); n3_v = true; n_len--; };
            if(op1 == '\0' && ops_len>0) { op1 = ops.front(); ops.pop(); ops_len--; };
            if(op2 == '\0' && ops_len>0) { op2 = ops.front(); ops.pop(); ops_len--; };
            if(op1 == '\0' && op2 == '\0'){
                if(will_mirrored_axis != '\0'){
                    if(will_mirrored_axis == 'x'){
                        return win_width - n1;
                    } else if(will_mirrored_axis == 'y'){
                        return win_height - n1;
                    };
                    sub_reason = (char*)"Unknown mirror axis";
                    goto _invalid_format;
                };
                return n1;
            };
            if((op1 == '+' || op1 == '-') && (op2 == '*' || op2 == '/')){
                n2 = apply_op(n2, n3, op2);
            } else {
                n1 = apply_op(n1, n2, op1);
                n2 = n3;
                op1 = op2;
            };
            n3_v = false;
            op2 = '\0';
            ops_len = ops.size();
        };

        sub_reason = (char*)"Should never exit from loop."; // No break in loop

        _invalid_format:
        printf("Invalid anchor (%s): %s\n", sub_reason, anchor);
        return 1;
    };
};
