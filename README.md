# n-gram tweet bot

This is a simple random text generator, with a couple twists. It collects an n-gram language model from a plain text source specified with a command line argument, and then produces the requested number of randomly-generated tweets.

## Getting Started

Run the following in a terminal to use the provided corpus* of tweets from @realDonaldTrump:
```bash
make
./a.out trumptweets.txt
```

*Data collected from http://www.trumptwitterarchive.com

## Features

- Handles punctuation and EOT (End-Of-Tweet) probabilistically along with words
- n can be any positive integer
- Tries to use hashtags, @-mentions, and URLs correctly

## Known Issues

- Does not respect the 140-character limit
- Does not handle URLs correctly
- Sometimes misplaces apostrophes and quote marks
