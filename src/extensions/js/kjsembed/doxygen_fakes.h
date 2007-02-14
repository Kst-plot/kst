
/** An object that supports signals and slots. */
class QObject {};
/** Base-class of all widgets. */
class QWidget : public QObject {};
/** A widget that has a frame. */
class QFrame : public QWidget {};

/** Namespace for the KParts framework. */
namespace KParts {
    /** A base-class for plugins that extend a KPart. */
    class Plugin : public QObject, public XMLGUIClient {};
    /** A base-class for KParts. */
    class Part : public QObject, public XMLGUIClient {};
    /** A base-class for KParts that provide a read-only view. */
    class ReadOnlyPart : public Part {};
};

/** Namespace for the KJS interpreter library. */
namespace KJS {
    /** Provides the implementation for a KJS object. */
    class ObjectImp {};
};


