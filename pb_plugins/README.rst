======================================
Autogenerator for the MAVSDK
======================================

Install instructions
~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    pip install -r requirements.txt
    pip install -e .


How to use it
~~~~~~~~~~~~~

.. code-block:: bash

    export TEMPLATE_PATH="$(pwd)/templates/swift"
    protoc --plugin=protoc-gen-custom=$(which dcsdkgen) \
            -I../protos/action \
            --custom_out=. \
            --custom_opt=swift \
            ../protos/action/action.proto

