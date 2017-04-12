###a tiny network library - one-loop-per-thread reactor model
***
An implementation inspired by [ 《Linux多线程服务端编程》 ](https://book.douban.com/subject/20471211/).

***
Demonstrations are in tests folder of net and base module.
chargen, daytime, echo, pingpong, filesrvr.
***
When I wrote chatter on netty, I learnt something about usage but the design was a black box.
Tried debug through the initial process, but that just told me what does it do.
Some parts started to make sense when I read the book. So I decide to implement a similar one. But not invent a new one. 
I find it is so hard to answer the question 'what's the difference between yours and muduo?' in a short and brief way.  
Model is the same.  
Model is the most important part of this kind of network library.  
Model is why reactor is pervasively used.  

When I wrote tinyNL, I logged some thoughts in comments. 
But I did not write a document which intends to demonstrate something.
If you can vividly recall the chapters in that book, differences will be obvious, I think.
But if not, I'd say that major divergence is formed by how to deal with closing.
And this can be seen in commit logs, too. So git diff is kind of source for answers.  
So I think maybe I should log some answers to questions like why certain mechanism is needed. 