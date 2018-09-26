#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <list>

using namespace std;

/**
 * Returns a map from M words to a list of words that can follow those M words,
 * based on examples from the corpus found in `file`.
 */
map< list<string>, vector<string> > get_words_map(ifstream* file, int M) {
    string word;
    string line;
    vector<string> words_vector;
    while (getline(*file, line)) {
        // iterate over each line in the file
        for (int i = 0; i < line.length(); i++) {
            // loop through the line, collecting words
            if (line[i] == '.' or line[i] == ',' or line[i] == ';') {
                // add whatever word was already being built,
                // and then add this punctuation as another word
                if (!word.empty()) {
                    words_vector.push_back(word);
                    word = "";
                }
                words_vector.push_back(".");
            } else if (line[i] == '&') {
                // ampersands are written as '&amp' in this corpus
                if (!word.empty()) {
                    words_vector.push_back(word);
                    word = "";
                }
                words_vector.push_back("&");
                i++;
                i++;
                i++;
                i++;
            } else if (line[i] == '@' or line[i] == '#' or (line[i] == 'h' and line[i+1] == 't' and line[i+2] == 't' and line[i+3] == 'p' and line[i+4] == 's')) {
                // when one of these characters are encountered, we want everything
                // that follows until the next whitespace is encountered. These are
                // hashtags, @-mentions, and URLs.
                if (!word.empty()) {
                    words_vector.push_back(word);
                    word = "";
                }
                while (line[i] != ' ' and line[i] != '\n') {
                    // continue until whitespace is reached
                    word += line[i];
                    i++;
                }
                words_vector.push_back(word);
                word = "";
            } else if (line[i] == '-' and line[i+1] == '-') {
                // we don't want to collect a double dash between words
                i++;
                if (!word.empty()) {
                    words_vector.push_back(word);
                    word = "";
                }
            } else {
                if (isalpha(line[i]) or isdigit(line[i]) or line[i] == '-' or line[i] == '\'' or line[i] == '"' or line[i] == L'’') {
                    // besides special cases above, these are all the characters we want in words
                    word += line[i];
                } else {
                    if (!word.empty()) {
                        words_vector.push_back(word);
                        word = "";
                    }
                }
            }
        }
        // save anything that's left
        if (!word.empty()) {
            words_vector.push_back(word);
            word = "";
        }
        // the last word of the last line needs to be considered an "ending" word
        words_vector.push_back("\n");
    }

    // now collect these words into a map from a vector of n-1 words to the
    // possible words that follow
    map< list<string>, vector<string> > words_map;

    // initialize the state with empty "words"
    list<string> previous;
    for (int i = 0; i < M; i++) {
        previous.push_back("");
    }

    for (auto it = words_vector.begin(); it != words_vector.end(); it++) {
        // this word follows the n-1 words that preceeded it
        words_map[previous].push_back(*it);
        if (*it == "\n") {
            // start over the state
            previous.clear();
            for (int j = 0; j < M; j++) {
                previous.push_back("");
            }
        } else {
            // update the state
            previous.push_back(*it);
            previous.pop_front();
        }
    }

    return words_map;
}

int main(int argc, char *argv[]) {

    // ensure that only a file name was given as input
    if (argc != 2) {
        cout << "Command line arguments are invalid." << endl;
        return 1;
    }

    // ensure the file is there
    ifstream data_file (argv[1]);
    if (!data_file.is_open()) {
        cout << "That file does not exist." << endl;
        return 1;
    }

    // create the map of words to the `n` words that follow,
    // where `n` is specified by the user
    int M;
    cout << "Enter the value of 'n' for n-grams (e.g. '2' for bigrams): ";
    cin >> M;
    while ( cin.fail() or M < 1 ) {
        // ensure the input was an integer that can be used for n-grams
        cout << "Input was not an integer. Try again: ";
        cin.clear();
        cin >> M;
    }

    M--; // a bigram looks at the previous word; an n-gram looks at the previous (n-1) words

    int tweets_wanted;
    cout << "How many tweets do you want?: ";
    cin >> tweets_wanted;
    while ( std::cin.fail() ) {
        cout << "Input was not an integer. Try again: ";
        cin.clear();
        cin >> tweets_wanted;
    }
    cout << endl;

    // create the words map from the file
    map< list<string>, vector<string> > words_map = get_words_map(&data_file, M);

    data_file.close();

    // prepare the state for generating new text
    list<string> previous;
    for (int i = 0; i < M; i++) {
        previous.push_back("");
    }

    // marks whether a space is needed before the next word printed.
    // sentences don't start with spaces, so `go` is initialized false
    bool go = false;

    int tweet_count = 0;
    srand( time(NULL) );
    // produce the requested number of tweets
    while (tweet_count < tweets_wanted) {
        // use a random index to choose the word to follow the previous n-1 words
        auto ind = static_cast<int>(rand() % words_map[previous].size());
        if (words_map[previous][ind] == "\n") {
            // if the random choice is a newline, end the tweet
            tweet_count++;
            cout << endl << endl;

            // start the state over
            previous.clear();
            for (int j = 0; j < M; j++) {
                previous.push_back("");
            }
            // a space character will not be needed next time
            go = false;
        } else if (words_map[previous][ind] == "." or words_map[previous][ind] == "," or words_map[previous][ind] == ";") {
            // if punctuation is found, print it 
            cout << words_map[previous][ind];
            // update state
            previous.push_back(words_map[previous][ind]);
            previous.pop_front();
        } else {
            // simply print the word
            if (go) {
                // a space is needed this time
                cout << " ";
            } else {
                // beginning of the sentence. A space will be needed next time
                go = true;
            }
            cout << words_map[previous][ind];
            // update state
            previous.push_back(words_map[previous][ind]);
            previous.pop_front();
        }
    }
    cout << endl;

    return 0;
}
