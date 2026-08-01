import QtQuick
import QtTest
import XdgIcon 1.0

TestCase {
    name: "XdgIconIntegration"
    when: windowShown

    Component {
        id: noPropFixture
        XdgIcon {}
    }

    Component {
        id: namedFixture
        Item {
            property alias icon: theIcon
            property alias foundSpy: foundSpyImpl
            property alias pathSpy: pathSpyImpl

            XdgIcon {
                id: theIcon
                name: "app"
                size: 64
            }

            SignalSpy { id: foundSpyImpl; target: theIcon; signalName: "foundChanged" }
            SignalSpy { id: pathSpyImpl; target: theIcon; signalName: "pathChanged" }
        }
    }

    Component {
        id: constructionFixture
        Item {
            property alias icon: theIcon
            property alias pathSpy: pathSpyImpl

            XdgIcon {
                id: theIcon
                name: "app"
                size: 64
                scale: 2
            }

            SignalSpy { id: pathSpyImpl; target: theIcon; signalName: "pathChanged" }
        }
    }

    function test_instantiate_no_properties() {
        var fx = noPropFixture.createObject(this)
        verify(fx !== null)
        wait(100)
        fx.destroy()
    }

    function test_named_resolves_in_fixture() {
        var fx = namedFixture.createObject(this)
        verify(fx.icon.found)
        verify(fx.icon.path.toString().endsWith("64x64/apps/app.png"),
               "expected 64x64/apps/app.png, got " + fx.icon.path.toString())
        fx.destroy()
    }

    function test_name_change_triggers_found_changed() {
        var fx = namedFixture.createObject(this)
        wait(100)
        fx.foundSpy.clear()
        fx.icon.name = "nonexistent"
        wait(100)
        verify(fx.foundSpy.count > 0, "foundChanged should fire on name change")
        fx.destroy()
    }

    function test_no_redundant_resolves_during_construction() {
        var fx = constructionFixture.createObject(this)
        var count = fx.pathSpy.count
        compare(count, 1, "pathChanged should fire once during construction (got " + count + ")")
        fx.destroy()
    }
}
