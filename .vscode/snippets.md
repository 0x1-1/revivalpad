# Useful snippets

Check source of signal

```cpp
    QObject *obj = this->sender();
    if (obj != nullptr)
        qInfo() << "objName ::" << obj->metaObject()->className() << endl
                << "method ::" << obj->metaObject()->method(senderSignalIndex()).methodSignature();
```

To make revivalpad crash when a warning happens use command:

`QT_FATAL_WARNINGS=1 ./build/bin/revivalpad`

(Useful when used with debugger)