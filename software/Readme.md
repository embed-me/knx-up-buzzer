# 🛠️ Software

The UP-Buzzer project integrates seamlessly with **ETS**, primarily tested with **ETS 6.6**. However, the project can be adapted to work with older ETS versions if required.



## Tools

To create the KNX product database and provide full flexibility, this project uses:

- **[Kaenx-Creator](https://github.com/OpenKNX/Kaenx-Creator)**: A versatile tool for generating KNX product databases.

### Recommended Version

Kaenx-Creator is under active development, and updates can sometimes introduce breaking changes. For this project, it is recommended to use **[v1.8.4](https://github.com/OpenKNX/Kaenx-Creator/tree/v1.8.4)** to ensure compatibility.

- **Binary Download**: [Kaenx Creator v1.8.4](https://github.com/OpenKNX/Kaenx-Creator/releases/download/v1.8.4/Kaenx.Creator.Binaries.zip)
- **MD5 Hash**: `9aeb971f7844eeafb54341031f3e4ef8`



## Generation

To modify or recreate the product database:

1. Open the file `up-buzzer.ae-manu`.
2. Make any necessary changes and increase the version number.
3. Re-deploy the database.

The generated outputs include:

- **`knxprod.h`**: Move this file to [firmware/src/drivers/knx/data/](firmware/src/drivers/knx/data/).

### Note

During re-deployment, the program may become temporarily unresponsive. Allow it some time, as it will eventually complete successfully.