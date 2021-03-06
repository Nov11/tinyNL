
[![Travis Status](https://travis-ci.org/Nov11/tinyNL.svg?branch=master)](https://travis-ci.org/Nov11/tinyNL)
### a tiny network library - one-loop-per-thread reactor model
***
An implementation inspired by [ 《Linux多线程服务端编程》 ](https://book.douban.com/subject/20471211/).

***
Demonstrations are in tests folders of net and base module.
chargen, daytime, echo, pingpong, filesrvr, etc.
***
When I wrote chatter on netty, I learnt something about usage but the design was a black box.
Tried debug through the initial process, but that just told me what does it do.
Some parts started to make sense when I read the book. So I decide to implement a similar one. **But not inventing a new one**. 
I find it is so hard to answer the question 'what's the difference between yours and muduo?' in a short and brief way.  

Model is the same.  
Model is the most important part of this kind of network library.  
Model is why reactor is pervasively used.  

When I wrote tinyNL, I logged some thoughts in comments. 
But I did not write a document which intends to demonstrate something.
If you can vividly recall the chapters in that book, differences will be obvious, I think.
But if not, I'd say that major divergence is formed by how to deal with closing.
And this can be seen in commit logs, too. So git diff is kind of source for answers.  
So I think maybe I should log some answers to questions like why certain mechanism is needed in the future.
Issues are welcome.

### Features
* 'app' directory resides one application for now. It has a runnable docker image.
* 'embedlua' directory resides integration of lua script which servers as bussiness logic processing. Handler is able to invoke lua script. And 'send' function is callable in lua script. This is a prototype. 


### NO codec bundled with this library
* codec is simply a string processing job other than the real function of network library
* netty's codec can be a helpful example. [this(a chat service)](https://github.com/Nov11/chatter) can be a reference.
* codec can be built by chaining handlers as a pipeline i.e. read length first and then convert the data
* protobuf can be a useful tool to do serialization/deserialization
* **I will not add codec for a long long time.** 
