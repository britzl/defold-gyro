# Gyro
This is a a gyroscope extension for the [Defold game engine](http://www.defold.com).

## Installation
You can use the Gyro extension in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

https://github.com/britzl/defold-gyro/archive/master.zip


## Usage
    function init(self)
      gyro.start(function(self, self, pitch, roll, yaw, quatx, quaty, quatz, quatw)
        go.set_rotation(vmath.quat(quatx, quaty, quatz, quatw))
      end)
    end
    
    function final(self)
      gyro.stop()
    end

## Limitations
The extension currently only supports iOS.